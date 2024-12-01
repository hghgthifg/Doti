export module Utils.Camera;

import std;
import Math;

export class Camera {
public:
    Camera() {
        this->reset();
    }

    auto getView() const -> Mat4 {
        const auto view = Math::lookAt(
            _pos,        // Camera position
            _pos + _dir, // Look at position
            _up          // Up vector
        );
        return view;
    }

    auto updateOrientation(float deltaX, float deltaY) -> void {
        constexpr float sensitivity = 1.1f;
        float           xoffset     = -deltaX * sensitivity;
        float           yoffset     = deltaY * sensitivity;

        _yaw += xoffset;
        _pitch = Math::clamp(_pitch + yoffset, -89.0f, 89.0f);

        this->updateDirection();
    }

    auto getFov() const -> float { return _fov; }
    auto setFov(const float fov) -> void { _fov = fov; }
    auto getPos() const -> Vec3 { return _pos; }

    auto reset() -> void {
        _pos   = Vec3(0.0f, 0.0f, 0.5f);
        _dir   = Vec3(0.0f, 0.0f, -1.0f);
        _up    = Vec3(0.0f, 1.0f, 0.0f);
        _yaw   = -90.0f;
        _pitch = 0.0f;
        _lastX = 800.0f / 2.0;
        _lastY = 600.0 / 2.0;
        _fov   = 45.0f;
    }

private:
    auto updateDirection() -> void {
        Vec3 front{
            Math::cos(Math::radians(_yaw)) * Math::cos(Math::radians(_pitch)),
            Math::sin(Math::radians(_pitch)),
            Math::sin(Math::radians(_yaw)) * Math::cos(Math::radians(_pitch))
        };
        _dir = Math::normalize(front);
    }

    Vec3  _pos;
    Vec3  _dir;
    Vec3  _up;
    float _yaw;
    float _pitch;
    float _lastX;
    float _lastY;
    float _fov;
};
