export module Utils.Camera;

import std;
import Math;
import Utils.Event;
import Debug.Logger;

export class Camera {
public:
    enum class MoveDirection {
        Forward,
        Backward,
        Left,
        Right,
    };

    Camera(const float screen_width, const float screen_height) {
        this->reset(screen_width, screen_height);
        EventManager::registerEvent<>("Camera::MoveLeft");
        EventManager::registerEvent<>("Camera::MoveRight");
        EventManager::registerEvent<>("Camera::MoveForward");
        EventManager::registerEvent<>("Camera::MoveBackward");
        EventManager::registerEvent<MoveDirection>("Camera::Move");
        EventManager::connect<>("Camera::MoveLeft", [this]() {
            _cameraPos -= _cameraRight * _cameraSpeed;
            EventManager::emit("Camera::Move", MoveDirection::Left);
        });
        EventManager::connect<>("Camera::MoveRight", [this]() {
            _cameraPos += _cameraRight * _cameraSpeed;
            EventManager::emit("Camera::Move", MoveDirection::Right);
        });
        EventManager::connect<>("Camera::MoveForward", [this]() {
            _cameraPos += _cameraFront * _cameraSpeed;
            EventManager::emit("Camera::Move", MoveDirection::Forward);
        });
        EventManager::connect<>("Camera::MoveBackward", [this]() {
            _cameraPos -= _cameraFront * _cameraSpeed;
            EventManager::emit("Camera::Move", MoveDirection::Backward);
        });
        EventManager::connect<MoveDirection>("Camera::Move", [this](MoveDirection) {
            EventManager::emit("Render::RefreshHistoryFramedata");
        });
    }

    auto getViewMatrix() const -> Mat4 {
        const auto view = Math::lookAt(
            _cameraPos,                // Camera position
            _cameraPos + _cameraFront, // Look at position
            _cameraUp                  // Up vector
        );
        return view;
    }

    auto updateOrientation(const float deltaX, const float deltaY) -> void {
        constexpr float sensitivity = 1.1f;
        float           Xoffset     = -deltaX * sensitivity;
        float           Yoffset     = deltaY * sensitivity;

        _yaw += Xoffset;
        _pitch = Math::clamp(_pitch + Yoffset, -89.0f, 89.0f);

        this->updateCameraVectors();
    }

    auto getFov() const -> float { return _fov; }

    auto setFov(const float fov) -> void {
        _fov        = fov;
        _halfHeight = Math::tan(Math::radians(_fov / 2.0f));
        _halfWidth  = _halfHeight * _screenRatio;

        _leftBottomCorner = _cameraFront - _halfWidth * _cameraRight - _halfHeight * _cameraUp;
    }

    auto getCameraPos() const -> Vec3 { return _cameraPos; }
    auto getCameraFront() const -> Vec3 { return _cameraFront; }
    auto getCameraUp() const -> Vec3 { return _cameraUp; }
    auto getCameraRight() const -> Vec3 { return _cameraRight; }
    auto getHalfWidth() const -> float { return _halfWidth; }
    auto getHalfHeight() const -> float { return _halfHeight; }
    auto getLeftBottomCorner() const -> Vec3 { return _leftBottomCorner; }

    auto reset(uint32_t screen_width, uint32_t screen_height) -> void {
        _yaw               = -90.0f;
        _pitch             = 0.0f;
        _fov               = 60.0f;
        _isFirstMouseInput = true;
        _cameraPos         = Vec3(0.0f, 0.0f, 3.0f);
        _worldUp           = Vec3(0.0f, 1.0f, 0.0f);

        _screenRatio = static_cast<float>(screen_width) / static_cast<float>(screen_height);
        _halfHeight  = Math::tan(Math::radians(_fov / 2.0f));
        _halfWidth   = _halfHeight * _screenRatio;
        _cameraSpeed = 1.0f;
        this->updateCameraVectors();
    }

    auto updateScreenRatio(int screen_width, int screen_height) -> void {
        _screenRatio = static_cast<float>(screen_width) / static_cast<float>(screen_height);
        _halfWidth   = _halfHeight * _screenRatio;

        _leftBottomCorner = _cameraFront - _halfWidth * _cameraRight - _halfHeight * _cameraUp;
    }

private:
    auto updateCameraVectors() -> void {
        const Vec3 front{
            Math::cos(Math::radians(_yaw)) * Math::cos(Math::radians(_pitch)),
            Math::sin(Math::radians(_pitch)),
            Math::sin(Math::radians(_yaw)) * Math::cos(Math::radians(_pitch))
        };
        _cameraFront = Math::normalize(front);
        _cameraRight = Math::normalize(Math::cross(_cameraFront, _worldUp));
        _cameraUp    = Math::normalize(Math::cross(_cameraRight, _cameraFront));

        _leftBottomCorner = _cameraFront - _halfWidth * _cameraRight - _halfHeight * _cameraUp;
    }

    Vec3  _cameraPos;
    Vec3  _cameraFront;
    Vec3  _cameraUp;
    Vec3  _cameraRight;
    float _cameraSpeed;

    Vec3 _worldUp;

    float _fov; // Degree
    float _yaw;
    float _pitch;

    bool  _isFirstMouseInput;
    float _lastX;
    float _lastY;

    float _screenRatio;
    float _halfWidth;
    float _halfHeight;
    Vec3  _leftBottomCorner;
};
