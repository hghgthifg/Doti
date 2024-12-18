export module Graphics.Render.Pipeline;

import std;
import OpenGL;
import Core.Math;
import Core.Logger;
import Graphics.FrameCanvas;
import Graphics.Shader;
import Graphics.Render.Acceleration.BVHTree;
import Graphics.Render.Acceleration.Primitive;
import Graphics.Render.RenderContext;
import Graphics.Type.Material;
import Graphics.Type.Light;
import Graphics.Buffer;

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

    void addPointLight(const PointLight& light) {
        _pointLights.emplace_back(light);
    }

    void addAreaLight(const AreaLight& light) {
        _areaLights.emplace_back(light);
    }

    void initialize() {
        buildBVH();
        _rasterGeoShader = Shaders::GetRasterizationGeometryPassShader();
        // _rasterColShader = Shaders::GetRasterizationGeometryPassShader();

        _indexBuffer.allocate(_indices.size() * sizeof(_indices[0]), _indices.data());
        _vertexBuffer.allocate(_vertices.size() * sizeof(_vertices[0]), _vertices.data());
        _normalBuffer.allocate(_normals.size() * sizeof(_normals[0]), _normals.data());
        _materialIDBuffer.allocate(_triangleMaterialIDs.size() * sizeof(_triangleMaterialIDs[0]),
                                   _triangleMaterialIDs.data());

        _materialBuffer.allocate(_materials.size() * sizeof(_materials[0]), _materials.data());
        _areaLightBuffer.allocate(_areaLights.size() * sizeof(_areaLights[0]), _areaLights.data());
        _pointLightBuffer.allocate(_pointLights.size() * sizeof(_pointLights[0]), _pointLights.data());

        _bvhBuffer = _bvh.genNodeBuffer();
        initraster();
    }

    void setContext(const std::shared_ptr<RenderContext>& context) { _renderContext = context; }

    void clearData() {
        _vertices.clear();
        _indices.clear();
        _normals.clear();

        _sphereCenters.clear();
        _sphereRadius.clear();

        _triangleMaterialIDs.clear();
        _sphereMaterialIDs.clear();

        _bvh = BVHTree();
        _bvhBuffer.reset();

        _indexBuffer.~Buffer();
        _vertexBuffer.~Buffer();
        _normalBuffer.~Buffer();
        _materialBuffer.~Buffer();
        _areaLightBuffer.~Buffer();
        _pointLightBuffer.~Buffer();
    }

    void render() {
        auto context = _renderContext.lock();

        rasterizationPass();
        if (context->getFrameCount() == 0) {}

        _frameCanvas.renderToScreen();
    }

    void initraster() {
        struct Matrices {
            Mat4 model;
            Mat4 view;
            Mat4 projection;
        };

        _uboMatrices.allocate(sizeof(Matrices), nullptr, 0, false, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboMatrices.getID());

        _uboMaterials.allocate(sizeof(Material) * 100, _materials.data(), 0, false, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uboMaterials.getID());

        // GLuint matricesIndex = glGetUniformBlockIndex(_rasterGeoShader.getID(), "Matrices");
        // if (matricesIndex == GL_INVALID_INDEX) {
        //     Logger::error("Unable to find Uniform Block 'Matrices'");
        // } else {
        glUniformBlockBinding(_rasterGeoShader.getID(), 0, 0);
        // }

        // GLuint materialsIndex = glGetUniformBlockIndex(_rasterGeoShader.getID(), "Materials");
        // if (materialsIndex == GL_INVALID_INDEX) {
        //     Logger::error("Unable to find Uniform Block 'Materials'");
        // } else {
        glUniformBlockBinding(_rasterGeoShader.getID(), 1, 1);
        // }

        glCreateVertexArrays(1, &_vao);
        glEnableVertexArrayAttrib(_vao, 0); // aPos
        glEnableVertexArrayAttrib(_vao, 1); // aNormal
        glEnableVertexArrayAttrib(_vao, 2); // aMaterialID

        glVertexArrayVertexBuffer(_vao, 0, _vertexBuffer.getID(), 0, sizeof(Vec3));
        glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(_vao, 0, 0);

        glVertexArrayVertexBuffer(_vao, 1, _normalBuffer.getID(), 0, sizeof(Vec3));
        glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(_vao, 1, 1);

        glVertexArrayVertexBuffer(_vao, 2, _materialIDBuffer.getID(), 0, sizeof(uint32_t));
        glVertexArrayAttribIFormat(_vao, 2, 1, GL_UNSIGNED_INT, 0);
        glVertexArrayAttribBinding(_vao, 2, 2);

        glVertexArrayElementBuffer(_vao, _indexBuffer.getID());
    }

    void rasterizationPass() {
        _frameCanvas.bindRasterizationGBuffer();
        _rasterGeoShader.activate();

        struct Matrices {
            Mat4 model;
            Mat4 view;
            Mat4 projection;
        };

        const auto& context = _renderContext.lock();
        const auto& camera  = context->getCamera();

        Matrices matrices;
        matrices.view       = camera.getViewMatrix();
        matrices.projection = camera.getProjectionMatrix();
        matrices.model      = Mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        _uboMatrices.write(0, sizeof(Matrices), &matrices);

        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, _indices.size() / 3);
        glBindVertexArray(0);

        _rasterGeoShader.deactivate();

        // _rasterColShader.activate();
        // _frameCanvas.bindRasterizationFBO();
        // _frameCanvas.drawQuad();
        // _rasterColShader.deactivate();
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

    GLuint _vao;

    Buffer                  _materialBuffer{};
    Buffer                  _vertexBuffer{};
    Buffer                  _indexBuffer{};
    Buffer                  _normalBuffer{};
    Buffer                  _pointLightBuffer{};
    Buffer                  _areaLightBuffer{};
    Buffer                  _materialIDBuffer{};
    Buffer                  _uboMatrices{};
    Buffer                  _uboMaterials{};
    std::shared_ptr<Buffer> _bvhBuffer;

    std::vector<Vec3>  _sphereCenters{};
    std::vector<float> _sphereRadius{};

    std::vector<Material> _materials{};

    std::vector<PointLight> _pointLights{};
    std::vector<AreaLight>  _areaLights{};

    std::vector<uint32_t> _triangleMaterialIDs{};
    std::vector<uint32_t> _sphereMaterialIDs{};

    BVHTree _bvh{};

    Shader      _rasterGeoShader{};
    Shader      _rasterColShader{};
    Shader      _rayTracingShader{};
    FrameCanvas _frameCanvas{};

    std::weak_ptr<RenderContext> _renderContext;
};
