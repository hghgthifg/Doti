export module Test.BallSpace;

import std;
import Core.Math;
import Scene.SceneBase;
import Graphics.Camera;
import Core.Event;
import Core.Logger;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Canvas;
import Graphics.Render.Drawable.Model;

auto setupScene(const Shader& shader) -> void {
    shader.activate();
    shader.setFloat("sphere[0].radius", 0.5);
    shader.setVec3("sphere[0].center", Vec3(0.0, 0.0, -1.0));
    shader.setInt("sphere[0].materialIndex", 0);
    shader.setVec3("sphere[0].albedo", Vec3(0.8, 0.7, 0.2));

    shader.setFloat("sphere[1].radius", 0.5);
    shader.setVec3("sphere[1].center", Vec3(1.0, 0.0, -1.0));
    shader.setInt("sphere[1].materialIndex", 1);
    shader.setVec3("sphere[1].albedo", Vec3(0.2, 0.7, 0.6));

    shader.setFloat("sphere[2].radius", 0.5);
    shader.setVec3("sphere[2].center", Vec3(-1.0, 0.0, -1.0));
    shader.setInt("sphere[2].materialIndex", 1);
    shader.setVec3("sphere[2].albedo", Vec3(0.1, 0.3, 0.7));

    shader.setFloat("sphere[3].radius", 0.5);
    shader.setVec3("sphere[3].center", Vec3(0.0, 0.0, 0.0));
    shader.setInt("sphere[3].materialIndex", 0);
    shader.setVec3("sphere[3].albedo", Vec3(0.9, 0.9, 0.9));

    shader.setVec3("tri[0].v0", Vec3(2.0, -0.5, 2.0));
    shader.setVec3("tri[0].v1", Vec3(-2.0, -0.5, -2.0));
    shader.setVec3("tri[0].v2", Vec3(-2.0, -0.5, 2.0));

    shader.setVec3("tri[1].v0", Vec3(2.0, -0.5, 2.0));
    shader.setVec3("tri[1].v1", Vec3(-2.0, -0.5, -2.0));
    shader.setVec3("tri[1].v2", Vec3(2.0, -0.5, -2.0));
    shader.deactivate();
}

export class BallSpace final : public SceneBase {
public:
    BallSpace() = default;

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
        bindEvents();
        auto shader = Shader(
            "BallSpace",
            "resource/shaders/BallSpace/vertex_shader.glsl",
            "resource/shaders/BallSpace/fragment_shader.glsl"
        );
        setupScene(shader);
        _camera.updateScreenRatio(_width, _height);
        _renderContext.setShader(std::move(shader));
    }

    void render() override {
        EventManager::emit("Scene::NewRenderLoop");
        _renderContext.setCamera(_camera);
        _renderContext.setFrameCount(_renderLoopCount);
        _canvas.draw(_renderContext);
    }

    void exit() override {
        EventManager::disconnectAll<Vec2>("Input::MouseDrag");
        EventManager::disconnectAll<float>("Input::MouseScroll");
        unbindEvents();
    }

private:
    Model  _model;
    Canvas _canvas;
};
