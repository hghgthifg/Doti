export module Graphics.Type.Light;

import Core.Math;

export struct PointLight {
    Vec3  position;
    Vec3  color;
    float intensity;
};

export struct AreaLight {
    Vec3  position;
    Vec3  normal;
    Vec2  size;
    Vec3  color;
    float intensity;
};
