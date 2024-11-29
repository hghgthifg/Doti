export module Utils.Camera;

import std;
import Math;

export class Camera {
public:
    auto getView() -> Mat4 {
        Mat4 view = Mat4(1.0f);
        view      = Math::lookAt(
            pos,                   // Camera position
            pos + dir,             // Look at position
            Vec3(0.0f, 1.0f, 0.0f) // Up vector
        );
        return view;
    }

private:
    Vec3 pos = Vec3(0.0f, 0.0f, 3.0f);
    Vec3 dir = Math::normalize(Vec3(0.0f, 0.0f, 0.0f) - pos);
};
