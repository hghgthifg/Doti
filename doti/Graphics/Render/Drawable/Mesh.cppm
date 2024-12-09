export module Graphics.Render.Drawable.Mesh;

import std;
import Math;
import OpenGL;
import Debug.Logger;
import Graphics.Shader;
import Graphics.Render.RenderContext;

/*!
 * @brief Structure representing a texture with its properties
 */
export struct MeshTexture {
    GLuint      id;   /*!< OpenGL texture ID */
    std::string type; /*!< Type of the texture (e.g., diffuse, specular) */
    std::string path; /*!< File path to the texture */
};

/*!
 * @brief Structure representing a vertex with multiple attributes
 */
export struct MeshVertex {
    Point3  position;   /*!< Position of the vertex in 3D space */
    Vec3    normal;     /*!< Normal vector of the vertex */
    Vec2    texCoords;  /*!< MeshTexture coordinates */
    Vec3    tangent;    /*!< Tangent vector for normal mapping */
    Vec3    bitangent;  /*!< Bitangent vector for normal mapping */
    GLint   boneIds[4]; /*!< Bone IDs for skeletal animation */
    GLfloat weights[4]; /*!< Weights for skeletal animation */
};

template<typename V>
concept VertexContainer = requires(V v)
{
    { v.begin() } -> std::input_iterator;
    { v.end() } -> std::input_iterator;
    typename std::remove_reference_t<V>::value_type;
    requires std::same_as<typename std::remove_reference_t<V>::value_type, MeshVertex>;
};

template<typename I>
concept IndexContainer = requires(I i)
{
    { i.begin() } -> std::input_iterator;
    { i.end() } -> std::input_iterator;
    typename std::remove_reference_t<I>::value_type;
    requires std::same_as<typename std::remove_reference_t<I>::value_type, uint32_t>;
};

template<typename T>
concept TextureContainer = requires(T t)
{
    { t.begin() } -> std::input_iterator;
    { t.end() } -> std::input_iterator;
    typename std::remove_reference_t<T>::value_type;
    requires std::same_as<typename std::remove_reference_t<T>::value_type, MeshTexture>;
};

/*!
 * @brief Class representing a drawable mesh in the graphics renderer
 */
export class Mesh {
public:
    Mesh() : _vertexTexture(0), _indexTexture(0), _isSetup(false) {}

    /*!
     * @brief Construct a new Mesh object with given vertices, indices and textures
     * @tparam V Container type of Vertex which can be iterated over to construct a vector
     * @tparam I Container type of uint32_t which can be iterated over to construct a vector
     * @tparam T Container type of Texture which can be iterated over to construct a vector
     * @param vertices Vertices of the mesh
     * @param indices Indices of the vertices to form triangles
     * @param textures Textures of the mesh
     * @note If you want to avoid copying, please use @code std::move@endcode when passing the arguments
     */
    template<VertexContainer V, IndexContainer I, TextureContainer T>
    Mesh(V&& vertices, I&& indices, T&& textures)
        : _vertices(std::forward<V>(vertices)),
          _indices(std::forward<I>(indices)),
          _textures(std::forward<T>(textures)) {
        this->setup();
    }

    /*!
     * @brief Set the vertices, indices and textures of the mesh
     * @tparam V Container type of Vertex which can be iterated over to construct a vector
     * @tparam I Container type of uint32_t which can be iterated over to construct a vector
     * @tparam T Container type of Texture which can be iterated over to construct a vector
     * @param vertices Vertices of the mesh
     * @param indices Indices of the vertices to form triangles
     * @param textures Textures of the mesh
     * @note If you want to avoid copying, please use @code std::move@endcode when passing the arguments
     */
    template<VertexContainer V, IndexContainer I, TextureContainer T>
    auto set(V&& vertices, I&& indices, T&& textures) -> void {
        _vertices = std::forward<V>(vertices);
        _indices  = std::forward<I>(indices);
        _textures = std::forward<T>(textures);
        this->setup();
    }

    /*!
     * @brief Draws the mesh using the provided shader
     * @param render_context Render context to be used for rendering the mesh
     */
    auto draw(RenderContext& render_context) const -> void {
        if (!_isSetup) {
            Logger::warning("Mesh has not been setup.");
            return;
        }

        const Shader& rawShader = render_context.directlyAccessShader(*this);
        rawShader.activate();

        render_context.apply();

        /*
        // uint32_t diffuseNr  = 1;
        // uint32_t specularNr = 1;
        // for (uint32_t i = 0; i < _textures.size(); i++) {
        //     glActiveTexture(static_cast<GLenum>(static_cast<uint32_t>(GL_TEXTURE0) + i));
        //     // Get texture id (N in diffuse_textureN)
        //     std::string number;
        //     std::string name = _textures[i].type;
        //     if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
        //     else if (name == "texture_specular") number = std::to_string(specularNr++);
        //     rawShader.setInt(("material." + name + number).c_str(), i);
        //     glBindTexture(GL_TEXTURE_2D, _textures[i].id);
        // }
        // glActiveTexture(GL_TEXTURE0);
        //
        // // Draw mesh
        // glBindVertexArray(_vao);
        // glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); */

        rawShader.setInt("meshVertexNum", _vertices.size());
        rawShader.setInt("meshIndexNum", _indices.size());
        rawShader.setInt("texMeshVertex", 1);
        rawShader.setInt("texMeshFaceIndex", 2);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _vertexTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _indexTexture);

        rawShader.deactivate();
    }

private:
    /*!
     * @brief Sets up the OpenGL buffers and vertex attributes for the mesh
     */
    auto setup() -> void {
        /*
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(MeshVertex), _vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                              (void *) __builtin_offsetof(MeshVertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                              (void *) __builtin_offsetof(MeshVertex, texCoords));

        glBindVertexArray(0);
        */

        // We want to store mesh data inside a texture, so that we can use mesh data in the shader.

        /* 1. Decide the size of texture we want to use. */
        const uint32_t verticesSize = _vertices.size();
        const uint32_t indicesSize  = _indices.size();

        float    vert_x_f = Math::sqrt(verticesSize);
        uint32_t vert_x   = Math::ceil(vert_x_f);
        uint32_t vert_y   = Math::ceil(static_cast<float>(verticesSize) / static_cast<float>(vert_x));

        float    face_x_f = Math::sqrt(indicesSize);
        uint32_t face_x   = Math::ceil(face_x_f);
        uint32_t face_y   = Math::ceil(static_cast<float>(indicesSize) / static_cast<float>(face_x));

        // TODO: Use PBO to avoid create a large vector here
        /* 2. Generate the texture. */
        std::vector<float> vertexArray;
        std::vector<float> faceArray;

        // Each vertex contains vertex coordinates (3 float), normal (3 float) and uv (2 float).
        vertexArray.resize((3 + 3 + 2) * (vert_x * vert_y));
        faceArray.resize((1) * (face_x * face_y));

        for (uint32_t i = 0; i < verticesSize; i++) {
            vertexArray[i * 8 + 0] = 0.04 * _vertices[i].position.x;
            vertexArray[i * 8 + 1] = 0.04 * _vertices[i].position.y;
            vertexArray[i * 8 + 2] = 0.04 * _vertices[i].position.z;

            vertexArray[i * 8 + 3] = _vertices[i].normal.x;
            vertexArray[i * 8 + 4] = _vertices[i].normal.y;
            vertexArray[i * 8 + 5] = _vertices[i].normal.z;

            vertexArray[i * 8 + 6] = _vertices[i].texCoords.x;
            vertexArray[i * 8 + 7] = _vertices[i].texCoords.y;
        }

        for (uint32_t i = 0; i < indicesSize; i++) {
            faceArray[i] = _indices[i];
        }

        /* 3. Send data to GPU */
        glGenTextures(1, &_vertexTexture);
        glBindTexture(GL_TEXTURE_2D, _vertexTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, vert_x * 8, vert_y, 0, GL_RED, GL_FLOAT, vertexArray.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenTextures(1, &_indexTexture);
        glBindTexture(GL_TEXTURE_2D, _indexTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, face_x, face_y, 0, GL_RED, GL_FLOAT, faceArray.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        _isSetup = true;
    }

    // GLuint                   _vao;      /*!< OpenGL MeshVertex Array Object handle */
    // GLuint                   _vbo;      /*!< OpenGL MeshVertex Buffer Object handle */
    // GLuint                   _ebo;      /*!< OpenGL Element Buffer Object handle */
    GLuint                   _vertexTexture;
    GLuint                   _indexTexture;
    bool                     _isSetup;  /*!< Flag indicating whether the mesh has been set up */
    std::vector<MeshVertex>  _vertices; /*!< Container holding the vertices of the mesh */
    std::vector<uint32_t>    _indices;  /*!< Container holding the indices of the mesh */
    std::vector<MeshTexture> _textures; /*!< Container holding the textures of the mesh */
};
