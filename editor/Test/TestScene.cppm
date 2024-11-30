export module Test.TestScene;

import std;
import Math;
import Scene.SceneBase;
import Utils.Camera;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Triangle;
import Graphics.Render.Drawable.Model;

export class TestScene : public SceneBase {
public:
    TestScene() {
        // std::vector points{
        //     Point3{-0.5f, -0.5f, 0.0f},
        //     Point3{0.5f, -0.5f, 0.0f},
        //     Point3{0.0f, 0.5f, 0.0f}
        // };
        // _triangle.setVertices(points);
    }

    void load() override {
        auto shader = Shader(
            "default",
            "resource/shaders/vertex_shader.glsl",
            "resource/shaders/fragment_shader.glsl"
        );
        _renderContext.setShader(std::move(shader));
        _bunnyModel = Model(
            "resource/models/bunny.obj"
        );
    }

    void render() override {
        auto projection = Math::perspective(Math::radians(_camera.getFov()), _width / _height, 0.1f, 100.0f);
        _renderContext.setProjectionMatrix(projection);

        auto view = _camera.getView();
        _renderContext.setViewMatrix(view);

        auto viewPos = _camera.getPos();
        _renderContext.setViewPos(viewPos);

        auto model = Mat4(1.0f);
        model      = Math::translate(model, Vec3{0.0f, 0.0f, 0.0f});
        model      = Math::scale(model, Vec3{1.0f, 1.0f, 1.0f});
        _renderContext.setModelMatrix(model);

        _bunnyModel.draw(_renderContext);
    }

    void exit() override {}

private:
    // Triangle _triangle;
    Model _bunnyModel;
};
