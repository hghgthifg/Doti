export module Test.Bunny;

import std;
import Math;
import Scene.SceneBase;
import Graphics.Camera;
import Utils.Event;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Triangle;
import Graphics.Render.Drawable.Model;

export class BunnyScene : public SceneBase {
public:
    BunnyScene() = default;

    void load() override {
        EventManager::connect<Vec2>("Input::MouseDrag", [this](const Vec2& delta) {
            _camera.updateOrientation(delta.x, delta.y);
        });
        EventManager::connect<float>("Input::MouseScroll", [this](float delta) {
            auto fov = _camera.getFov();
            _camera.setFov(Math::clamp(fov - delta, 0.1f, 80.0f));
        });
        _rasterizationShader = Shader(
            "default",
            "resource/shaders/rasterization.vert",
            "resource/shaders/rasterization.frag"
        );
        _bunnyModel = Model(
            "resource/models/bunny.obj"
        );
        _camera.updateScreenRatio(_width, _height);
        // _renderContext.setCamera(_camera);
        // _renderContext.setShader(std::move(shader));
        // _camera.reset();
    }

    void render() override {
        auto projection = Math::perspective(Math::radians(_camera.getFov()), _width / _height, 0.1f, 100.0f);
        // _renderContext.setProjectionMatrix(projection);

        auto view = _camera.getViewMatrix();
        // _renderContext.setViewMatrix(view);

        auto model = Mat4(1.0f);
        model      = Math::translate(model, Vec3{0.0f, 0.0f, 0.0f});
        model      = Math::scale(model, Vec3{1.0f, 1.0f, 1.0f});
        // _renderContext.setModelMatrix(model);

        _rasterizationShader.setMat4("model", model);
        _rasterizationShader.setMat4("view", view);
        _rasterizationShader.setMat4("projection", projection);

        // _nanosuitModel.draw(_renderContext);
    }

    void exit() override {
        EventManager::disconnectAll<Vec2>("Input::MouseDrag");
        EventManager::disconnectAll<float>("Input::MouseScroll");
    }

private:
    // Triangle _triangle;
    Shader _rasterizationShader;
    Model  _bunnyModel;
};
