export module Graphics.Render.Object.Mesh;

import std;
import OpenGL;
import Core.Math;
import Graphics.Render.ObjectBase;
import Graphics.Render.Acceleration.Primitive;
import Graphics.Render.RenderContext;

export class Mesh final : public ObjectBase {
public:
    Mesh(std::vector<Point3>&&   vertices,
         std::vector<Vec3>&&     normals,
         std::vector<uint32_t>&& indices,
         uint32_t&&              material_ids
    ) : ObjectBase(),
        _vertices(std::move(vertices)),
        _normals(std::move(normals)),
        _indices(std::move(indices)),
        _materialID(std::move(material_ids)) {};

    // auto getVertices() const -> std::vector<Point3> { return _vertices; };
    // auto getNormals() const -> std::vector<Vec3> { return _normals; };
    // auto getIndices() const -> std::vector<uint32_t> { return _indices; };
    // auto getTextures() const -> std::vector<Texture> { return _textures; };

    auto sendDataToPipeline() -> void override {
        auto& pipeline = Pipeline::getInstance();
        pipeline.sendTriangleData(_vertices, _normals, _indices, _materialID);
    }

private:
    std::vector<Point3>   _vertices;
    std::vector<Vec3>     _normals;
    std::vector<uint32_t> _indices;
    uint32_t              _materialID;
    // std::vector<uint32_t> _textures;
};
