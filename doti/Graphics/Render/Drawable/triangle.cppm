export module Graphics.Render.Drawable.Triangle;

import std;
import Math;
import OpenGL;
import Graphics.Shader;
import Debug.Logger;

/*!
 * @brief Structure representing a vertex with a position
 */
struct Vertex {
    Point3 position; /*!< Position of the vertex in 3D space */
};

/*!
 * @brief Concept to ensure that the provided vertex container meets the requirements for a Triangle
 * @tparam V The type of the vertex container
 */
template<typename V>
concept TriangleVertex = requires(V v)
{
    { v.begin() } -> std::input_iterator;
    { v.end() } -> std::input_iterator;
    typename std::remove_reference_t<V>::value_type;
    requires std::same_as<typename std::remove_reference_t<V>::value_type, Point3>;
};

/*!
 * @brief Class representing a drawable triangle in the graphics renderer
 */
export class Triangle {
public:
    /*!
     * @brief Default constructor initializing OpenGL handles and setup flag
     */
    Triangle() : _vao(0), _vbo(0), _isSetup(false) {}

    /*!
     * @brief Constructor that initializes the triangle with a set of vertices
     * @tparam V Type of the vertex container
     * @param vertices A container of vertices that satisfies the TriangleVertex concept
     */
    template<TriangleVertex V>
    Triangle(V&& vertices)
        : _vertices(std::forward<V>(vertices)) {
        this->setup();
    }

    /*!
     * @brief Sets the vertices of the triangle and initializes the setup
     * @tparam V Type of the vertex container
     * @param vertices A container of vertices that satisfies the TriangleVertex concept
     */
    template<TriangleVertex V>
    auto set(V&& vertices) -> void {
        _vertices = std::vector<Vertex>(std::make_move_iterator(vertices.begin()),
                                        std::make_move_iterator(vertices.end()));
        this->setup();
    }

    /*!
     * @brief Draws the triangle using the provided shader
     * @param shader The shader to be used for rendering the triangle
     */
    auto draw(const Shader& shader) const -> void {
        if (!_isSetup) {
            Logger::warning("Triangle has not been setup.");
            return;
        }
        shader.activate();
        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        shader.deactivate();
    }

private:
    /*!
     * @brief Sets up the OpenGL buffers and vertex attributes for the triangle
     */
    auto setup() -> void {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        _isSetup = true;
    }

    uint32_t            _vao;      /*!< OpenGL Vertex Array Object handle */
    uint32_t            _vbo;      /*!< OpenGL Vertex Buffer Object handle */
    bool                _isSetup;  /*!< Flag indicating whether the triangle has been set up */
    std::vector<Vertex> _vertices; /*!< Container holding the vertices of the triangle */
};
