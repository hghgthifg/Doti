export module Graphics.Render.Object.Mesh;

import std;
import OpenGL;
import Core.Math;
import Graphics.Render.ObjectBase;
import Graphics.Render.Acceleration.Primitive;
import Graphics.Render.RenderContext;

export class Mesh final : public ObjectBase {
public:
    Mesh(std::vector<Vertex>&&  vertices,
         std::vector<Index>&&   indices,
         std::vector<Texture>&& textures) : ObjectBase(),
                                            _vertices(std::move(vertices)),
                                            _indices(std::move(indices)),
                                            _textures(std::move(textures)) {};

    auto getVertices() const -> std::vector<Vertex> { return _vertices; };
    auto getIndices() const -> std::vector<Index> { return _indices; };
    auto getTextures() const -> std::vector<Texture> { return _textures; };

    auto load(RenderContext& render_context) -> void override {
        std::vector<Vec3>     positions;
        std::vector<Vec3>     normals;
        std::vector<uint32_t> indices;
        positions.reserve(_vertices.size());
        normals.reserve(_indices.size() / 3);
        for (uint32_t i = 0; i < _indices.size() / 3; i++) {
            auto primitive = std::make_shared<TrianglePrimitive>(
                _vertices[_indices[3 * i + 0]].position,
                _vertices[_indices[3 * i + 1]].position,
                _vertices[_indices[3 * i + 2]].position
            );
            render_context.addPrimitive(primitive);
            normals.emplace_back(
                (_vertices[_indices[3 * i + 0]].normal +
                 _vertices[_indices[3 * i + 1]].normal +
                 _vertices[_indices[3 * i + 2]].normal) * (1.0f / 3.0f)
            );
        }
        for (auto vertex: _vertices) {
            positions.emplace_back(vertex.position);
        }
        indices.assign(_indices.begin(), _indices.end());
        render_context.addMeshData(std::move(positions), std::move(normals), std::move(indices));
    }

private:
    std::vector<Vertex>  _vertices;
    std::vector<Index>   _indices;
    std::vector<Texture> _textures;
};
