export module Test.TestScene;

import std;
import Math;
import Debug.Logger;
import Scene.SceneBase;
import Graphics.Camera;
import Utils.Event;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Triangle;
import Graphics.Render.Drawable.Model;

export class TestScene : public SceneBase {
public:
    TestScene() = default;

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
        // _nanosuitModel = Model(
        //     "resource/models/nanosuit/nanosuit.obj"
        // );
        // _bunnyModel = Model(
        //     "resource/models/bunny.obj"
        // );
        _cornellBoxModel = Model(
            "resource/models/cornell-box/cornell-box.obj"
        );
        _camera.updateScreenRatio(_width, _height);
        _renderContext.setCamera(_camera);
    }

    void render() override {
        /* 1. Rasterization */
        _renderContext.setShader(_rasterizationShader);
        auto model      = Mat4(1.0f);
        auto view       = _camera.getViewMatrix();
        auto projection = Math::perspective(Math::radians(_camera.getFov()), _width / _height, 0.1f, 100.0f);
        // model           = Math::scale(model, Vec3{0.5f, 0.5f, 0.5f});
        // model = Math::translate(model, Vec3(0.0f, -1.0f, 0.0f));

        _rasterizationShader.activate();
        _rasterizationShader.setMat4("model", model);
        _rasterizationShader.setMat4("view", view);
        _rasterizationShader.setMat4("projection", projection);
        _rasterizationShader.deactivate();

        _cornellBoxModel.draw(_renderContext);
        // _rasterizationShader.activate();
        // _rasterizationShader.setMat4("model", model);
        // _rasterizationShader.setMat4("view", view);
        // _rasterizationShader.setMat4("projection", projection);
        // _rasterizationShader.deactivate();
        //
        // _nanosuitModel.draw(_renderContext);
        //
        // model = Math::translate(model, Vec3{0.0f, 0.0f, 0.0f});
        // model = Math::scale(model, Vec3{1.0f, 1.0f, 1.0f});
        // _rasterizationShader.activate();
        // _rasterizationShader.setMat4("model", model);
        // _rasterizationShader.setMat4("view", view);
        // _rasterizationShader.setMat4("projection", projection);
        // _rasterizationShader.deactivate();
        //
        // _bunnyModel.draw(_renderContext);

        /* 2. Global Illumination */
    }

    void exit() override {
        EventManager::disconnectAll<Vec2>("Input::MouseDrag");
        EventManager::disconnectAll<float>("Input::MouseScroll");
    }

private:
    Shader _rasterizationShader;
    Model  _nanosuitModel;
    Model  _bunnyModel;
    Model  _cornellBoxModel;
};
