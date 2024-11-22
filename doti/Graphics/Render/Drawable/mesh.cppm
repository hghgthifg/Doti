export module Graphics.Render.Drawable.Mesh;

import std;
import Math;
import OpenGL;
import Graphics.Shader;
import Debug.Logger;

/*!
 * @brief Structure representing a vertex with multiple attributes
 */
struct Vertex {
    Point3  position;   /*!< Position of the vertex in 3D space */
    Vec3    normal;     /*!< Normal vector of the vertex */
    Vec2    texCoords;  /*!< Texture coordinates */
    Vec3    tangent;    /*!< Tangent vector for normal mapping */
    Vec3    bitangent;  /*!< Bitangent vector for normal mapping */
    GLint   boneIds[4]; /*!< Bone IDs for skeletal animation */
    GLfloat weights[4]; /*!< Weights for skeletal animation */
};

/*!
 * @brief Structure representing a texture with its properties
 */
struct Texture {
    uint32_t    id;   /*!< OpenGL texture ID */
    std::string type; /*!< Type of the texture (e.g., diffuse, specular) */
    std::string path; /*!< File path to the texture */
};

template<typename V>
concept VertexContainer = requires(V v)
{
    { v.begin() } -> std::input_iterator;
    { v.end() } -> std::input_iterator;
    typename V::value_type;
    requires std::same_as<typename V::value_type, Vertex>;
};

template<typename I>
concept IndexContainer = requires(I i)
{
    { i.begin() } -> std::input_iterator;
    { i.end() } -> std::input_iterator;
    typename I::value_type;
    requires std::same_as<typename I::value_type, uint32_t>;
};

template<typename T>
concept TextureContainer = requires(T t)
{
    { t.begin() } -> std::input_iterator;
    { t.end() } -> std::input_iterator;
    typename T::value_type;
    requires std::same_as<typename T::value_type, Texture>;
};

/*!
 * @brief Class representing a drawable mesh in the graphics renderer
 */
export class Mesh {
public:
    Mesh() : _vao(0), _vbo(0), _ebo(0), _isSetup(false) {}

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

    // /*!
    //  * @brief Add more vertices, indices and textures to the mesh
    // * @tparam V Container type of Vertex which can be iterated over to construct a vector
    //  * @tparam I Container type of uint32_t which can be iterated over to construct a vector
    //  * @tparam T Container type of Texture which can be iterated over to construct a vector
    //  * @param Vertices you want to add to the mesh
    //  * @param Indices of the vertices to form triangles you want to add to the mesh
    //  * @param Textures you want to add to the mesh
    //  */
    // template<VertexContainer V, IndexContainer I, TextureContainer T>
    // void add(V&& vertices, I&& indices, T&& textures) {
    //     _vertices.insert(_vertices.end(), std::forward<V>(vertices).begin(), std::forward<V>(vertices).end());
    //     _indices.insert(_indices.end(), std::forward<I>(indices).begin(), std::forward<I>(indices).end());
    //     _textures.insert(_textures.end(), std::forward<T>(textures).begin(), std::forward<T>(textures).end());
    // }

    /*!
     * @brief Draws the mesh using the provided shader
     * @param shader The shader to be used for rendering the mesh
     */
    auto draw(const Shader& shader) const -> void {
        if (!_isSetup) {
            Logger::warning("Mesh has not been setup.");
            return;
        }
        uint32_t diffuseNr  = 1;
        uint32_t specularNr = 1;
        for (uint32_t i = 0; i < _textures.size(); i++) {
            glActiveTexture(static_cast<GLenum>(static_cast<uint32_t>(GL_TEXTURE0) + i)); // 在绑定之前激活相应的纹理单元
            /* Get texture id (N in diffuse_textureN)*/
            std::string number;
            std::string name = _textures[i].type;
            if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
            else if (name == "texture_specular") number = std::to_string(specularNr++);

            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, _textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        /* Draw mesh */
        glBindVertexArray(_vao);
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    /*!
     * @brief Sets up the OpenGL buffers and vertex attributes for the mesh
     */
    auto setup() -> void {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) __builtin_offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) __builtin_offsetof(Vertex, texCoords));

        glBindVertexArray(0);

        _isSetup = true;
    }

    uint32_t              _vao;      /*!< OpenGL Vertex Array Object handle */
    uint32_t              _vbo;      /*!< OpenGL Vertex Buffer Object handle */
    uint32_t              _ebo;      /*!< OpenGL Element Buffer Object handle */
    bool                  _isSetup;  /*!< Flag indicating whether the mesh has been set up */
    std::vector<Vertex>   _vertices; /*!< Container holding the vertices of the mesh */
    std::vector<uint32_t> _indices;  /*!< Container holding the indices of the mesh */
    std::vector<Texture>  _textures; /*!< Container holding the textures of the mesh */
};
