export module Test.TestScene;

import std;
import Math;
import Scene.SceneBase;
import Graphics.Shader;
import Graphics.Render.Drawable.Triangle;

export class TestScene : public SceneBase {
public:
    TestScene(): _shader("default", "resource/shaders/vertex_shader.glsl", "resource/shaders/fragment_shader.glsl") {
        std::vector points{
            Point3{-0.5f, -0.5f, 0.0f},
            Point3{0.5f, -0.5f, 0.0f},
            Point3{0.0f, 0.5f, 0.0f}
        };
        _triangle.setVertices(points);
    }

    void render() override {
        _triangle.draw(_shader);
    }

    void exit() override {}

private:
    Triangle _triangle;
    Shader   _shader;
};
