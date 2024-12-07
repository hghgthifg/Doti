export module Test.TestScene;

import std;
import Math;
import Scene.SceneBase;
import Graphics.Camera;
import Utils.Event;
import Debug.Logger;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Canvas;
import Graphics.Render.Drawable.Model;

export class TestScene : public SceneBase {
public:
    TestScene() = default;

    void load() override {
        EventManager::connect<Vec2>("Input::MouseDrag", [this](const Vec2& delta) {
            _camera.updateOrientation(delta.x, delta.y);
            EventManager::emit("Render::RefreshHistoryFramedata");
        });
        EventManager::connect<float>("Input::MouseScroll", [this](float delta) {
            const auto fov = _camera.getFov();
            _camera.setFov(Math::clamp(fov - delta, 0.1f, 80.0f));
            EventManager::emit("Render::RefreshHistoryFramedata");
        });
        auto shader = Shader(
            "Default",
            "resource/shaders/PhongShadingScene/vertex_shader.glsl",
            "resource/shaders/PhongShadingScene/fragment_shader.glsl"
        );
        auto model = Model("resource/models/cornell-box.obj");
        _camera.updateScreenRatio(_width, _height);
        // _renderContext.setShader(std::move(shader));
        // _renderContext.addModel(model);
    }

    void render() override {
        EventManager::emit("Scene::NewRenderLoop");
        _renderContext.setCamera(_camera);
        _renderContext.setFrameCount(_renderLoopCount);

        /* 1. Rasterization */
        _model.draw(_renderContext);

        // _canvas.draw(_renderContext);
    }

    void exit() override {
        EventManager::disconnectAll<Vec2>("Input::MouseDrag");
        EventManager::disconnectAll<float>("Input::MouseScroll");
    }

private:
    Model  _model;
    Canvas _canvas;
};
