export module Test.TestScene;

import std;
import Math;
import Scene.SceneBase;
import Utils.Camera;
import Graphics.Shader;
import Graphics.Render.Drawable.Triangle;

export class TestScene : public SceneBase {
public:
    TestScene(): _shader("default", "resource/shaders/vertex_shader.glsl", "resource/shaders/fragment_shader.glsl") {
        // Mat4 projection = Math::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        // _shader.setMat4("projection", projection);
        std::vector points{
            Point3{-0.5f, -0.5f, 0.0f},
            Point3{0.5f, -0.5f, 0.0f},
            Point3{0.0f, 0.5f, 0.0f}
        };
        _triangle.setVertices(points);
    }

    void render() override {
        // auto view = _camera.getView();
        // _shader.setMat4("view", view);

        //get time
        // auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
        // std::chrono::system_clock::now().time_since_epoch());

        // model      = Math::rotate(model, (float) time.count(), Vec3(0.0f, 0.0f, 1.0f));
        Mat4 model = Mat4(1.0f);
        _shader.setMat4("model", model);

        _triangle.draw(_shader);
    }

    void exit() override {}

private:
    Camera   _camera;
    Triangle _triangle;
    Shader   _shader;
};
