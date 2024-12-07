export module Graphics.Render.Drawable.Model;

import assimp;
import std;
import Math;
import OpenGL;
import Debug.Logger;
import Graphics.Render.RenderContext;
import Graphics.Loader.Image;
import Graphics.Render.Drawable.Mesh;
import Graphics.Shader;

static JXLImageLoader ImageLoader;

export class Model {
public:
    Model() {}

    Model(const std::string& path) : _directory(path) {
        this->loadModel(path);
    }

    auto draw(RenderContext& render_context) const -> void {
        for (auto mesh: _meshes) {
            mesh.draw(render_context);
        }
    }

private:
    std::vector<Mesh>        _meshes;
    std::vector<MeshTexture> _textures;
    std::filesystem::path    _directory;

    // TODO: Asynchronous loading
    auto loadModel(const std::string& path) -> void {
        Logger::info("Loading model: " + path);
        Assimp::Importer       importer;
        const Assimp::aiScene* scene = importer.ReadFile(
            path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::error("Failed to load model: " + path);
            return;
        }

        if (!ImageLoader.init()) {
            Logger::error("Failed to init image loader!");
            return;
        }
        processNode(scene->mRootNode, scene);
    }

    auto processNode(Assimp::aiNode* node, const Assimp::aiScene* scene) -> void {
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            Assimp::aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh, scene));
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    auto processMesh(Assimp::aiMesh* mesh, const Assimp::aiScene* scene) -> Mesh {
        // Data to fill
        std::vector<MeshVertex>  vertices;
        std::vector<uint32_t>    indices;
        std::vector<MeshTexture> textures;

        // Walk through each of the mesh's vertices
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            MeshVertex vertex;
            /*
             * We declare a placeholder vector since assimp uses its own vector class that
             * doesn't directly convert to glm's vec3 class so we transfer the data to this
             * placeholder glm::vec3 first positions
             */
            Vec3 vector;

            vector          = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            vertex.position = vector;

            if (mesh->HasNormals()) {
                vector        = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
                vertex.normal = vector;
            }

            if (mesh->mTextureCoords[0]) {
                /*
                 * a vertex can contain up to 8 different texture coordinates.
                 * We thus make the assumption that we won't use models where a vertex
                 * can have multiple texture coordinates so we always take the first set (0).
                 */
                Vec2 vec;
                vec              = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                vertex.texCoords = vec;
                // tangent
                vector         = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                vertex.tangent = vector;
                // bitangent
                vector           = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                vertex.bitangent = vector;
            } else {
                vertex.texCoords = Vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }
        // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            Assimp::aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        // process materials
        Assimp::aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN
        // TODO: Bulk loading
        // 1. diffuse maps
        std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                                    "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                                     "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                                   "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<MeshTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                                   "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        Mesh result;
        result.set(vertices, indices, textures);
        return result;
    }

    auto loadMaterialTextures(Assimp::aiMaterial* material, Assimp::aiTextureType type,
                              const std::string&  typeName) -> std::vector<MeshTexture> {
        std::vector<MeshTexture> textures;
        for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
            Assimp::aiString str;
            material->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            auto iter = std::find_if(_textures.begin(), _textures.end(),
                                     [&str](const MeshTexture& texture) {
                                         return std::strcmp(texture.path.c_str(), str.C_Str()) == 0;
                                     });

            if (iter != _textures.end()) {
                textures.push_back(*iter);
                continue;
            }

            MeshTexture texture;
            texture.id   = loadTextureFromFile(str.C_Str(), _directory.parent_path());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            _textures.push_back(texture);
        }
        return textures;
    }

    auto loadTextureFromFile(const char* name, const std::filesystem::path& directory) -> GLuint {
        const std::filesystem::path texture_filename = directory / name;

        GLuint textureID;
        glGenTextures(1, &textureID);

        // TODO: Now the image data is not released when an error occurs or after use.
        const auto index = ImageLoader.loadImages(std::vector{texture_filename.string()});
        if (const auto opt = ImageLoader.getImage(index[0]); opt.has_value()) {
            const auto& [data, width, height, components, filename, loaded] = opt->get();
            if (!loaded) {
                Logger::error("Failed to load image: " + filename + " : incomplete image");
                return 0;
            }
            if (data.empty()) {
                Logger::error("Failed to load image: " + filename + " : empty image");
                return 0;
            }
            GLenum format;
            if (components == 1) {
                format = GL_RED;
            } else if (components == 3) {
                format = GL_RGB;
            } else if (components == 4) {
                format = GL_RGBA;
            } else {
                Logger::error("Failed to load image: " + filename + " : invalid format");
                return 0;
            }
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            Logger::warning(std::format("{} is empty!", texture_filename.string()));
        }

        return textureID;
    }
};
