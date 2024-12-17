export module Graphics.Render.Pipeline;

import std;
import Core.Math;
import Graphics.FrameCanvas;
import Graphics.Shader;
import Graphics.Render.Acceleration.BVHTree;
import Graphics.Render.Acceleration.Primitive;
import Graphics.Render.RenderContext;
import Graphics.Type.Material;
import Graphics.Type.Light;

import Graphics.ShaderCollection;

export class Pipeline {
public:
    static Pipeline& getInstance() {
        static Pipeline instance;
        return instance;
    }

    // void sendVertices(const std::vector<Point3>& vertices) {
    //     _vertices.insert(_vertices.end(), vertices.begin(), vertices.end());
    // }
    //
    // void sendVertex(const Point3& vertex) { _vertices.emplace_back(vertex); }
    //
    // void sendIndices(const std::vector<uint32_t>& indices) {
    //     _indices.insert(_indices.end(), indices.begin(), indices.end());
    // }
    //
    // void sendIndex(uint32_t index) { _indices.emplace_back(index); }
    //
    // void sendNormals(const std::vector<Vec3>& normals) {
    //     _normals.insert(_normals.end(), normals.begin(), normals.end());
    // }
    //
    // void sendNormal(const Vec3& normal) { _normals.emplace_back(normal); }

    void sendTriangleData(const std::vector<Point3>&   vertices,
                          const std::vector<Vec3>&     normals,
                          const std::vector<uint32_t>& indices,
                          uint32_t                     materialID) {
        size_t triangleCount = indices.size() / 3;
        _vertices.insert(_vertices.end(), vertices.begin(), vertices.end());
        _normals.insert(_normals.end(), normals.begin(), normals.end());
        _indices.insert(_indices.end(), indices.begin(), indices.end());
        _triangleMaterialIDs.insert(_triangleMaterialIDs.end(), triangleCount, materialID);
    }

    void sendSingleSphereData(const Vec3& center, float radius, uint32_t materialID) {
        _sphereCenters.emplace_back(center);
        _sphereRadius.emplace_back(radius);
        _sphereMaterialIDs.emplace_back(materialID);
    }

    void addMaterial(const Material& material) {
        _materials.emplace_back(material);
    }

    auto initialize() -> void {
        buildBVH();
        _rasterShader = Shaders::GetRasterizationShader();
    }

    auto setContext(const std::shared_ptr<RenderContext>& context) -> void { _renderContext = context; }

    auto clearData() -> void {
        _vertices.clear();
        _indices.clear();
        _normals.clear();

        _sphereCenters.clear();
        _sphereRadius.clear();

        _triangleMaterialIDs.clear();
        _sphereMaterialIDs.clear();

        _bvh = BVHTree();
    }

    auto render() -> void {
        auto context = _renderContext.lock();

        if (context->getFrameCount() == 0) { rasterizationPass(); }

        _frameCanvas.renderToScreen();
    }

    auto rasterizationPass() -> void {
        _rasterShader.activate();

        const auto& context = _renderContext.lock();
        const auto& camera  = context->getCamera();

        const auto modelMatrix = Math::identity<Mat4>();
        const auto viewMatrix  = camera.getViewMatrix();
        const auto perpMatrix  = camera.getProjectionMatrix();

        _rasterShader.setMat4("model", modelMatrix);
        _rasterShader.setMat4("view", viewMatrix);
        _rasterShader.setMat4("projection", perpMatrix);

        _rasterShader.deactivate();
    }

private:
    void buildBVH() {
        const int                                   numTriangles = _indices.size() / 3;
        const int                                   numSpheres   = _sphereCenters.size();
        std::vector<std::shared_ptr<PrimitiveBase>> primitives;
        std::vector<std::shared_ptr<PrimitiveBase>> spherePrimitives(numSpheres);
        primitives.resize(numTriangles + numSpheres); // Reserve memory to accelerate

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                #pragma omp parallel for default(none) shared(_vertices, _indices, primitives, numTriangles)
                for (int i = 0; i < numTriangles; i++) {
                    Vec3 v0       = _vertices[_indices[i * 3 + 0]];
                    Vec3 v1       = _vertices[_indices[i * 3 + 1]];
                    Vec3 v2       = _vertices[_indices[i * 3 + 2]];
                    primitives[i] = std::make_shared<TrianglePrimitive>(v0, v1, v2);
                }
            }
            #pragma omp section
            {
                #pragma omp parallel for default(none) shared(_sphereCenters, _sphereRadius, spherePrimitives, numSpheres)
                for (int i = 0; i < numSpheres; i++) {
                    Vec3  center        = _sphereCenters[i];
                    float radius        = _sphereRadius[i];
                    spherePrimitives[i] = std::make_shared<SpherePrimitive>(center, radius);
                }
            }
        }

        primitives.insert(primitives.end(), std::make_move_iterator(spherePrimitives.begin()),
                          std::make_move_iterator(spherePrimitives.end()));

        _bvh = BVHTree(primitives);
    }

    Pipeline() = default;

    Pipeline(const Pipeline&) = delete;

    Pipeline& operator=(const Pipeline&) = delete;

    std::vector<Vec3>     _vertices{};
    std::vector<Vec3>     _normals{};
    std::vector<uint32_t> _indices{};

    std::vector<Vec3>  _sphereCenters{};
    std::vector<float> _sphereRadius{};

    std::vector<Material> _materials{};

    std::vector<uint32_t> _triangleMaterialIDs{};
    std::vector<uint32_t> _sphereMaterialIDs{};

    BVHTree _bvh{};

    Shader      _rasterShader{};
    Shader      _rayTracingShader{};
    FrameCanvas _frameCanvas{};

    std::weak_ptr<RenderContext> _renderContext;
};
