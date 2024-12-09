export module Graphics.Loader.Model;

import assimp;
import std;
import Math;
import OpenGL;
import Debug.Logger;
import Graphics.Material;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Mesh;
import Graphics.Render.Drawable.Model;
import Graphics.Loader.Image.Jxl;

// Model loading status
export struct ModelLoadStatus {
    bool        success{false};
    std::string message;
    float       progress{0.0f};
};

export class ModelLoader {
public:
    using LoadCallback = std::function<void(ModelLoadStatus&)>;

    enum class LoadOptions {
        GenerateTangents = 1 << 0,
        FlipUVs          = 1 << 1,
        OptimizeMeshes   = 1 << 2
    };

    struct LoadConfig {
        uint32_t                    options;
        std::optional<LoadCallback> progressCallback;

        LoadConfig() : options(static_cast<uint32_t>(LoadOptions::FlipUVs)) {}

        [[nodiscard]] bool hasOption(LoadOptions option) const noexcept {
            return (options & static_cast<uint32_t>(option)) != 0;
        }
    };

    static auto loadAsync(std::string_view path, LoadConfig config = {})
        -> std::future<std::optional<ModelData>> {
        return std::async(std::launch::async, [path = std::string(path), config]() {
            ModelLoader loader;
            return loader.load(path, config);
        });
    }

    auto load(std::string_view path, const LoadConfig& config = LoadConfig{}) -> std::optional<ModelData> {
        ModelData result;
        _directory = std::filesystem::path(path).parent_path();

        Assimp::Importer importer;
        uint32_t         flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals;

        if (config.hasOption(LoadOptions::FlipUVs)) flags |= aiProcess_FlipUVs;
        if (config.hasOption(LoadOptions::GenerateTangents)) flags |= aiProcess_CalcTangentSpace;
        if (config.hasOption(LoadOptions::OptimizeMeshes)) flags |= aiProcess_OptimizeMeshes;

        const Assimp::aiScene* scene = importer.ReadFile(std::string(path), flags);

        if (!scene || !scene->mRootNode) {
            reportProgress(config, 0.0f, "Failed to load model" + std::string(path));
            return std::nullopt;
        }

        result.materials.reserve(scene->mNumMaterials);
        result.meshes.reserve(scene->mNumMeshes);

        // Process materials first
        processMaterials(scene, config, result);

        // Process node hierarchy
        result.rootNode = processNodes(scene->mRootNode, scene, config, result);

        reportProgress(config, 1.0f, "Model loaded successfully");
        return result;
    }

private:
    auto processMaterials(const Assimp::aiScene* scene,
                          const LoadConfig&      config,
                          ModelData&             result) -> void {
        const auto totalMaterials = scene->mNumMaterials;

        for (uint32_t i = 0; i < totalMaterials; ++i) {
            // Progress reporting
            reportProgress(config,
                           static_cast<float>(i) / totalMaterials * 0.5f,
                           "Processing materials...");

            _imageLoader.init();
            try {
                auto*    material = scene->mMaterials[i];
                Material mat(material->GetName().C_Str());

                // Process PBR properties
                Assimp::aiColor3D color;
                float             value;

                if (material->Get("$clr.diffuse", 0, 0, color) == AI_SUCCESS) {
                    mat.setProperty(MaterialPropertyType::BaseColor,
                                    Vec3(color.r, color.g, color.b));
                }

                if (material->Get("$mat.roughnessFactor", 0, 0, value) == AI_SUCCESS) {
                    mat.setProperty(MaterialPropertyType::Roughness, value);
                }

                if (material->Get("$mat.metallicFactor", 0, 0, value) == AI_SUCCESS) {
                    mat.setProperty(MaterialPropertyType::Metallic, value);
                }

                // Process textures
                processTexture(material, aiTextureType_DIFFUSE,
                               MaterialPropertyType::BaseColor, mat);
                processTexture(material, aiTextureType_NORMALS,
                               MaterialPropertyType::Normal, mat);
                processTexture(material, aiTextureType_METALNESS,
                               MaterialPropertyType::Metallic, mat);
                processTexture(material, aiTextureType_DIFFUSE_ROUGHNESS,
                               MaterialPropertyType::Roughness, mat);
                processTexture(material, aiTextureType_AMBIENT_OCCLUSION,
                               MaterialPropertyType::AmbientOcclusion, mat);
                processTexture(material, aiTextureType_EMISSIVE,
                               MaterialPropertyType::Emissive, mat);
                processTexture(material, aiTextureType_OPACITY,
                               MaterialPropertyType::Opacity, mat);

                result.materials.push_back(std::move(mat));
            } catch (const std::exception& e) {
                Logger::warning(std::format("Failed to process material {}: {}", i, e.what()));
                // Add default material as fallback
                result.materials.push_back(Material::createDefaultMaterial());
            }
        }
    }

    auto setDefaultTexture(MaterialPropertyType propertyType, Material& mat) -> void {
        switch (propertyType) {
            case MaterialPropertyType::BaseColor: mat.setProperty(propertyType, Vec4(0.8f, 0.8f, 0.8f, 1.0f));
                break;
            case MaterialPropertyType::Normal: mat.setProperty(propertyType, Vec4(0.5f, 0.5f, 1.0f, 1.0f));
                break;
            case MaterialPropertyType::Metallic: mat.setProperty(propertyType, Vec4(0.0f));
                break;
            case MaterialPropertyType::Roughness: mat.setProperty(propertyType, Vec4(0.5f));
                break;
            case MaterialPropertyType::AmbientOcclusion: mat.setProperty(propertyType, Vec4(1.0f));
                break;
            default: mat.setProperty(propertyType, Vec4(1.0f));
                break;
        }
    }

    auto processTexture(Assimp::aiMaterial*   material,
                        Assimp::aiTextureType type,
                        MaterialPropertyType  propertyType,
                        Material&             mat) -> void {
        if (material->GetTextureCount(type) > 0) {
            Assimp::aiString path;
            if (material->GetTexture(type, 0, &path) == AI_SUCCESS) {
                try {
                    auto texPath = _directory / path.C_Str();

                    if (!std::filesystem::exists(texPath)) {
                        Logger::warning(std::format("Texture {} does not exist", path.C_Str()));
                        setDefaultTexture(propertyType, mat);
                    }

                    GLuint texId = loadTexture(texPath.string());
                    if (texId == 0) {
                        Logger::warning(std::format("Failed to load texture: {}", texPath.string()));
                        setDefaultTexture(propertyType, mat);
                        return;
                    }

                    mat.setProperty(propertyType, texId, texPath.string());
                } catch (const std::exception& e) {
                    Logger::warning(std::format("Failed to process texture: {}", e.what()));
                    setDefaultTexture(propertyType, mat);
                }
            }
        }
    }

    auto processNodes(const Assimp::aiNode*  node,
                      const Assimp::aiScene* scene,
                      const LoadConfig&      config,
                      ModelData&             result) -> std::unique_ptr<ModelNode> {
        auto currentNode  = std::make_unique<ModelNode>();
        currentNode->name = node->mName.C_Str();

        const auto t           = reinterpret_cast<const float *>(&node->mTransformation);
        currentNode->transform = Math::transpose(Mat4(
            t[0], t[1], t[2], t[3],
            t[4], t[5], t[6], t[7],
            t[8], t[9], t[10], t[11],
            t[12], t[13], t[14], t[15]
        ));

        currentNode->meshIndices.reserve(node->mNumMeshes);
        for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
            const auto meshIndex = node->mMeshes[i];
            if (meshIndex >= result.meshes.size()) {
                result.meshes.push_back(processMesh(scene->mMeshes[meshIndex]));
            }
            currentNode->meshIndices.push_back(meshIndex);
        }

        currentNode->children.reserve(node->mNumChildren);
        for (uint32_t i = 0; i < node->mNumChildren; ++i) {
            auto childNode = processNodes(node->mChildren[i], scene, config, result);
            currentNode->children.push_back(std::move(childNode));
        }

        return currentNode;
    }

    auto processMesh(Assimp::aiMesh* mesh) -> Mesh {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t>   indices;

        // Reserve space
        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        // Process vertices
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            MeshVertex vertex{};

            // Position
            vertex.position = Point3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

            // Normal
            if (mesh->HasNormals()) {
                vertex.normal = Vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }

            // TexCoords
            if (mesh->mTextureCoords[0]) {
                vertex.texCoords = Vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );

                // Tangent
                if (mesh->HasTangentsAndBitangents()) {
                    vertex.tangent = Vec3(
                        mesh->mTangents[i].x,
                        mesh->mTangents[i].y,
                        mesh->mTangents[i].z
                    );

                    vertex.bitangent = Vec3(
                        mesh->mBitangents[i].x,
                        mesh->mBitangents[i].y,
                        mesh->mBitangents[i].z
                    );
                }
            } else {
                vertex.texCoords = Vec2(0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            const auto& face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Create and return mesh
        return Mesh(std::move(vertices), std::move(indices), std::vector<MeshTexture>{});
    }

    auto loadTexture(const std::string& path) -> GLuint {
        // Check texture cache first
        if (auto it = _textureCache.find(path); it != _textureCache.end()) {
            return it->second;
        }

        // Load image using JXL loader
        const auto imageIndex = _imageLoader.appendImage(path);
        const auto imageOpt   = _imageLoader.getImage(imageIndex);

        if (!imageOpt) {
            Logger::error(std::format("Failed to load texture: {}", path));
            return 0;
        }

        const auto& image = imageOpt->get();

        // Generate OpenGL texture
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     static_cast<GLsizei>(image.width),
                     static_cast<GLsizei>(image.height),
                     0, GL_RGBA, GL_UNSIGNED_BYTE,
                     image.pixels.data());

        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Store in cache
        _textureCache[path] = textureId;

        return textureId;
    }

    void reportProgress(const LoadConfig&  config, float progress,
                        const std::string& status) {
        if (config.progressCallback) {
            ModelLoadStatus loadStatus;
            loadStatus.success  = progress == 1.0f;
            loadStatus.message  = status;
            loadStatus.progress = progress;

            (*config.progressCallback)(loadStatus);
        }
    }

    std::filesystem::path                   _directory;
    std::unordered_map<std::string, GLuint> _textureCache;
    JXLImageLoader                          _imageLoader;
};

