export module Test.TestScene;

import std;
import Math;
import Scene.SceneBase;
import Utils.Camera;
import Utils.Event;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Canvas;
import Graphics.Render.Drawable.Model;

export class TestScene : public SceneBase {
public:
    TestScene() = default;

    void load() override {
        EventManager::connect<Vec2>("MouseDrag", [this](const Vec2& delta) {
            _camera.updateOrientation(delta.x, delta.y);
        });
        EventManager::connect<float>("MouseScroll", [this](float delta) {
            auto fov = _camera.getFov();
            _camera.setFov(Math::clamp(fov - delta, 0.1f, 45.0f));
        });
        auto shader = Shader(
            "default",
            "resource/shaders/TestScene/vertex_shader.glsl",
            "resource/shaders/TestScene/fragment_shader.glsl"
        );
        _renderContext.setShader(std::move(shader));
        _camera.reset();
    }

    void render() override {
        _canvas.draw(_renderContext);
    }

    void exit() override {
        EventManager::disconnectAll<Vec2>("MouseDrag");
        EventManager::disconnectAll<float>("MouseScroll");
    }

private:
    Canvas _canvas;
};
