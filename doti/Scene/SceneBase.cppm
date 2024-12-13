export module Scene.SceneBase;

import std;
import Json;
import Core.Event;
import Core.Logger;
import Graphics.Camera;
import Graphics.Render.RenderContext;

export class SceneBase {
public:
    SceneBase() {
        if (!_eventsRegistered) {
            EventManager::registerEvent<>("Render::RefreshHistoryFramedata");
            EventManager::registerEvent<>("Scene::NewRenderLoop");
            _eventsRegistered = true;
        }
    }

    virtual ~SceneBase() = default;

    virtual void load() = 0;

    virtual void render() = 0;

    virtual void exit() = 0;

    inline auto setSize(const float width, const float height) -> void {
        _width  = width;
        _height = height;
        _camera.updateScreenRatio(_width, _height);
    }

protected:
    auto bindEvents() -> void {
        EventManager::connect<>("Camera::MoveLeft", [this]() {
            this->_camera.moveLeft();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Camera::MoveRight", [this]() {
            this->_camera.moveRight();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Camera::MoveForward", [this]() {
            this->_camera.moveForward();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Camera::MoveBackward", [this]() {
            this->_camera.moveBackward();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Scene::NewRenderLoop", [this] {
            this->_renderLoopCount = this->_renderLoopCount + 1;
        });
        EventManager::connect<>("Render::RefreshHistoryFramedata", [this] {
            this->_renderLoopCount = 0;
        });
    }

    auto unbindEvents() -> void {
        EventManager::disconnectAll<>("Camera::MoveLeft");
        EventManager::disconnectAll<>("Camera::MoveRight");
        EventManager::disconnectAll<>("Camera::MoveForward");
        EventManager::disconnectAll<>("Camera::MoveBackward");
        EventManager::disconnectAll<>("Scene::NewRenderLoop");
        EventManager::disconnectAll<>("Render::RefreshHistoryFramedata");
    }

    auto loadScene(const std::filesystem::path& path) -> void {
        std::ifstream file(path);
        if (!file.is_open()) {
            Logger::info("Failed to open scene file" + path.filename().string());
            return;
        }
    }

    auto saveScene(const std::filesystem::path& path) -> void {}

    static bool   _eventsRegistered;
    int32_t       _renderLoopCount = 0;
    float         _width           = 800, _height = 600;
    Camera        _camera{_width, _height};
    RenderContext _renderContext{};
};

bool SceneBase::_eventsRegistered = false;

