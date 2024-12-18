export module Graphics.Type.Light;

import Core.Math;

export struct PointLight {
    Vec3 position;
    Vec3 emission;
};

export struct AreaLight {
    Vec3 position;
    Vec3 normal;
    Vec3 emission;
    Vec2 size;
};
