export module Graphics.Render.RenderContext;

import std;
import OpenGL;
import Core.Math;
import Graphics.Buffer;
import Graphics.Camera;
import Core.Event;
import Core.Random;
import Core.Logger;

export class RenderContext {
public:
    RenderContext() {
        activate();
    }

    ~RenderContext() {
        deactivate();
    }

    auto activate() -> void {
        connectSlots();
    }

    auto deactivate() -> void {
        disconnectSlots();
    }

private:
    auto connectSlots() -> void;

    auto disconnectSlots() -> void;

    Camera   _camera{};
    uint32_t _frameCount = 0;
};

auto RenderContext::connectSlots() -> void {
    EventManager::connect<>("Camera::MoveLeft", [this]() {
        this->_camera.moveLeft();
        EventManager::emit("Render::RefreshHistoryFramedata");
    });
    EventManager::connect<>("Camera::MoveRight", [this]() {
        this->_camera.moveRight();
        EventManager::emit("Render::RefreshHistoryFramedata");
    });
    EventManager::connect<>("Camera::MoveForward", [this]() {
        this->_camera.moveForward();
        EventManager::emit("Render::RefreshHistoryFramedata");
    });
    EventManager::connect<>("Camera::MoveBackward", [this]() {
        this->_camera.moveBackward();
        EventManager::emit("Render::RefreshHistoryFramedata");
    });
    EventManager::connect<float, float>("Camera::UpdateOrientation", [this](float delta_x, float delta_y) {
        _camera.updateOrientation(delta_x, delta_y);
        EventManager::emit("Render::RefreshHistoryFramedata");
    });
    EventManager::connect<float>("Camera::UpdateFov", [this](float delta) {
        const auto fov = _camera.getFov();
        _camera.setFov(Math::clamp(fov - delta, 0.1f, 80.0f));
        EventManager::emit("Render::RefreshHistoryFramedata");
    });
    EventManager::connect<>("Render::NewFrame", [this] {
        this->_frameCount = this->_frameCount + 1;
    });
    EventManager::connect<>("Render::RefreshHistoryFramedata", [this] {
        this->_frameCount = 0;
    });
}

auto RenderContext::disconnectSlots() -> void {
    EventManager::disconnectAll<>("Camera::MoveLeft");
    EventManager::disconnectAll<>("Camera::MoveRight");
    EventManager::disconnectAll<>("Camera::MoveForward");
    EventManager::disconnectAll<>("Camera::MoveBackward");
    EventManager::disconnectAll<>("Camera::UpdateOrientation");
    EventManager::disconnectAll<>("Camera::UpdateFov");
    EventManager::disconnectAll<>("Render::NewFrame");
    EventManager::disconnectAll<>("Render::RefreshHistoryFramedata");
}
