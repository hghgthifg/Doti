export module Graphics.Render.Model;

import assimp;
import std;
import Math;
import OpenGL;
import Debug.Logger;
import Graphics.Render.Drawable.Mesh;
import Graphics.Shader;

export class Model {
public:
    Model(const std::string& path) {
        this->loadModel(path);
    }

    auto draw(const Shader& shader) const -> void {
        for (auto mesh: _meshes) {
            mesh.draw(shader);
        }
    }

private:
    std::vector<Mesh> _meshes;
    std::string       _directory;

    auto loadModel(const std::string& path) -> void {
        Logger::info("Loading model: " + path);
    }

    // auto processNode(aiNode* node, const aiScene* scene) -> void {}
    //
    // auto processMesh(aiMesh* mesh, const aiScene* scene) -> Mesh {}
    //
    // auto loadMaterialTextures(aiMaterial*        mat, aiTextureType type,
    //                           const std::string& typeName) -> std::vector<Texture> {}
};
