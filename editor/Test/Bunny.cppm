export module Test.Bunny;

/*
import std;
import Math;
import Scene.SceneBase;
import Utils.Camera;
import Utils.Event;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Triangle;
import Graphics.Render.Drawable.Model;

export class BunnyScene : public SceneBase {
public:
    BunnyScene() = default;

    void load() override {
        EventManager::connect<Vec2>("MouseDrag", [this](const Vec2& delta) {
            _camera.updateOrientation(delta.x, delta.y);
        });
        EventManager::connect<float>("MouseScroll", [this](float delta) {
            auto fov = _camera.getFov();
            _camera.setFov(Math::clamp(fov - delta, 0.1f, 45.0f));
        });
        auto shader = Shader(
            "BunnyShader (Phong)",
            "resource/shaders/BunnyScene/vertex_shader.glsl",
            "resource/shaders/BunnyScene/fragment_shader.glsl"
        );
        _renderContext.setShader(std::move(shader));
        _bunnyModel = Model(
            "resource/models/bunny.obj"
        );
        _camera.reset();
    }

    void render() override {
        auto projection = Math::perspective(Math::radians(_camera.getFov()), _width / _height, 0.1f, 100.0f);
        _renderContext.setProjectionMatrix(projection);

        auto view = _camera.getViewMatrix();
        _renderContext.setViewMatrix(view);

        auto viewPos = _camera.getPos();
        _renderContext.setViewPos(viewPos);

        auto model = Mat4(1.0f);
        model      = Math::translate(model, Vec3{0.0f, 0.0f, 0.0f});
        model      = Math::scale(model, Vec3{1.0f, 1.0f, 1.0f});
        _renderContext.setModelMatrix(model);

        _bunnyModel.draw(_renderContext);
    }

    void exit() override {
        EventManager::disconnectAll<Vec2>("MouseDrag");
        EventManager::disconnectAll<float>("MouseScroll");
    }

private:
    // Triangle _triangle;
    Model _bunnyModel;
};
*/
