export module Graphics.Render.RenderContext;

import std;
import OpenGL;
import Core.Math;
import Graphics.Shader;
import Graphics.Buffer;
import Graphics.Camera;
import Core.Event;
import Core.Random;
import Core.Logger;
import Graphics.Render.Acceleration.Primitive;
import Graphics.Render.Acceleration.BVHTree;

export class RenderContext {
public:
    auto apply() -> void {
        if (!_camera.has_value()) {
            Logger::warning("Camera not set! ");
        } else {
            const auto& camera = _camera->get();
            _shader.setInt("historyTexture", 0);
            _shader.setVec3("camera.position", camera.getCameraPos());
            _shader.setVec3("camera.front", camera.getCameraFront());
            _shader.setVec3("camera.right", camera.getCameraRight());
            _shader.setVec3("camera.up", camera.getCameraUp());
            _shader.setFloat("camera.halfHeight", camera.getHalfHeight());
            _shader.setFloat("camera.halfWidth", camera.getHalfWidth());
            _shader.setVec3("camera.leftBottom", camera.getLeftBottomCorner());
            _shader.setInt("frameCount", _frameCount.value());
            _shader.setFloat("randOrigin", 674764.0f * (Random::randFloat() + 1.0f));
        }
    }

    auto setup() -> void {
        _bvh       = BVHTree(_primitives);
        _bvhBuffer = _bvh.genNodeBuffer();

        _triangleIndicesBuffer  = std::make_shared<Buffer>();
        _triangleNormalsBuffer  = std::make_shared<Buffer>();
        _triangleVerticesBuffer = std::make_shared<Buffer>();
        _triangleAlbedosBuffer  = std::make_shared<Buffer>();

        _triangleIndicesBuffer->allocate(
            _triangleIndices.size() * sizeof(decltype(_triangleIndices[0])),
            _triangleIndices.data(),
            0
        );
        _triangleVerticesBuffer->allocate(
            _triangleVertices.size() * sizeof(decltype(_triangleVertices[0])),
            _triangleVertices.data(),
            0
        );
        _triangleNormalsBuffer->allocate(
            _triangleNormals.size() * sizeof(decltype(_triangleNormals[0])),
            _triangleNormals.data(),
            0
        );
        _triangleAlbedosBuffer->allocate(
            _triangleAlbedos.size() * sizeof(decltype(_triangleAlbedos[0])),
            _triangleAlbedos.data(),
            0
        );

        _shader.activate();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _triangleIndicesBuffer->getID());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _triangleVerticesBuffer->getID());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _triangleNormalsBuffer->getID());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _triangleAlbedosBuffer->getID());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, _bvhBuffer->getID());

        _shader.deactivate();
    }

    auto getShader() -> const Shader& {
        return _shader;
    }

    auto setShader(Shader&& shader) -> void {
        _shader = std::move(shader);
    }

    auto setCamera(const Camera& camera) -> void {
        _camera = camera;
    }

    auto setFrameCount(const int32_t& count) -> void {
        _frameCount = count;
    }

    auto addPrimitive(std::shared_ptr<PrimitiveBase> primitive) -> void {
        _primitives.emplace_back(primitive);
    }

    auto addMeshData(std::vector<Vec3>&& vertices, std::vector<Vec3>&& normals, std::vector<uint32_t>&& indices,
                     std::vector<Vec3>&& albedos) {
        for (auto& i: indices) {
            i += _triangleVertices.size();
        }
        for (uint32_t i = 0; i < vertices.size(); i++) {
            _triangleVertices.emplace_back(Vec4(vertices[i], 0.0f));
        }
        for (uint32_t i = 0; i < normals.size(); i++) {
            _triangleNormals.emplace_back(Vec4(normals[i], 0.0f));
        }
        for (uint32_t i = 0; i < indices.size() / 3; i++) {
            _triangleIndices.emplace_back(Vec4(
                indices[3 * i + 0],
                indices[3 * i + 1],
                indices[3 * i + 2],
                0
            ));
        }
        for (uint32_t i = 0; i < albedos.size(); i++) {
            _triangleAlbedos.emplace_back(Vec4(albedos[i], 0.0f));
        }
        // for (auto i: _triangleVertices) {
        //     std::cout << std::format("({}, {}, {}, {})", i.x, i.y, i.z, i.w) << " ";
        // }
        // std::cout << "\n";
        // for (auto i: _triangleNormals) {
        //     std::cout << std::format("({}, {}, {}, {})", i.x, i.y, i.z, i.w) << " ";
        // }
        // std::cout << "\n";
        // for (auto i: _triangleIndices) {
        //     std::cout << std::format("({}, {}, {}, {})", i.x, i.y, i.z, i.w) << " ";
        // }
        // std::cout << "\n";
    }

private:
    static bool                                          _eventsRegistered;
    bool                                                 _sceneUpdated = false;
    Shader                                               _shader{};
    std::optional<std::reference_wrapper<const Camera>>  _camera{};
    std::optional<std::reference_wrapper<const int32_t>> _frameCount{};

    std::vector<Vec4>  _triangleVertices{};
    std::vector<Vec4>  _triangleNormals{};
    std::vector<Vec4>  _triangleAlbedos{};
    std::vector<Vec4u> _triangleIndices{};

    std::shared_ptr<Buffer> _triangleVerticesBuffer{};
    std::shared_ptr<Buffer> _triangleNormalsBuffer{};
    std::shared_ptr<Buffer> _triangleIndicesBuffer{};
    std::shared_ptr<Buffer> _triangleAlbedosBuffer{};
    std::shared_ptr<Buffer> _bvhBuffer{};

    std::vector<std::shared_ptr<PrimitiveBase>> _primitives{};
    BVHTree                                     _bvh{};
};

bool RenderContext::_eventsRegistered = false;
