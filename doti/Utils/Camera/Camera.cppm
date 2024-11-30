export module Utils.Camera;

import std;
import Math;

export class Camera {
public:
    auto getView() const -> Mat4 {
        const auto view = Math::lookAt(
            _pos,        // Camera position
            _pos + _dir, // Look at position
            _up          // Up vector
        );
        return view;
    }

    auto getFov() const -> float { return _fov; }
    auto getPos() const -> Vec3 { return _pos; }

private:
    Vec3  _pos   = Vec3(0.0f, 0.0f, 3.0f);
    Vec3  _dir   = Vec3(0.0f, 0.0f, -1.0f);
    Vec3  _up    = Vec3(0.0f, 1.0f, 0.0f);
    float _yaw   = -90.0f;
    float _pitch = 0.0f;
    float _lastX = 800.0f / 2.0;
    float _lastY = 600.0 / 2.0;
    float _fov   = 45.0f;
};
