export module Graphics.Render.Object.Mesh;

import std;
import OpenGL;
import Core.Math;
import Graphics.Render.ObjectBase;
import Graphics.Render.Acceleration.Primitive;
import Graphics.Render.RenderContext;

export class Mesh final : public ObjectBase {
public:
    Mesh(std::vector<Vec3>&&     vertices,
         std::vector<Vec3>&&     normals,
         std::vector<uint32_t>&& indices
    ) : ObjectBase(),
        _vertices(std::move(vertices)),
        _normals(std::move(normals)),
        _indices(std::move(indices)) {};

    auto getVertices() const -> std::vector<Vec3> { return _vertices; };
    auto getNormals() const -> std::vector<Vec3> { return _normals; };
    auto getIndices() const -> std::vector<uint32_t> { return _indices; };
    // auto getTextures() const -> std::vector<Texture> { return _textures; };

    auto sendDataToPipeline() -> void override {
        auto& pipeline = Pipeline::getInstance();
        // if (_vertices.size() > 10000) {
        //     #pragma omp parallel sections default(none) shared(pipeline, _indices, _normals, _vertices)
        //     {
        //         #pragma omp section
        //         {
        //             pipeline.sendIndices(_indices);
        //         }
        //         #pragma omp section
        //         {
        //             pipeline.sendNormals(_normals);
        //         }
        //         #pragma omp section
        //         {
        //             pipeline.sendVertices(_vertices);
        //         }
        //     }
        // } else {
        pipeline.sendIndices(_indices);
        pipeline.sendNormals(_normals);
        pipeline.sendVertices(_vertices);
        // }
    }

private:
    std::vector<Vec3>     _vertices;
    std::vector<Vec3>     _normals;
    std::vector<uint32_t> _indices;
    // std::vector<uint32_t> _textures;
};
