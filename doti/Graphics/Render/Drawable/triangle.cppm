export module Graphics.Render.Drawable.Triangle;

import std;
import Math;
import OpenGL;
import Graphics.Shader;
import Debug.Logger;

struct Vertex {
    Point3 position;
};

template<typename V>
concept TriangleVertex = requires(V v)
{
    { v.begin() } -> std::input_iterator;
    { v.end() } -> std::input_iterator;
    typename std::remove_reference_t<V>::value_type;
    requires std::same_as<typename std::remove_reference_t<V>::value_type, Point3>;
};

export class Triangle {
public:
    Triangle() : _vao(0), _vbo(0), _isSetup(false) {}

    template<TriangleVertex V>
    Triangle(V&& vertices)
        : _vertices(std::forward<V>(vertices)) {
        this->setup();
    }

    template<TriangleVertex V>
    auto set(V&& vertices) -> void {
        _vertices = std::vector<Vertex>(std::make_move_iterator(vertices.begin()),
                                        std::make_move_iterator(vertices.end()));
        this->setup();
    }

    auto draw(const Shader& shader) -> void {
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

    uint32_t            _vao;
    uint32_t            _vbo;
    bool                _isSetup;
    std::vector<Vertex> _vertices;
};
