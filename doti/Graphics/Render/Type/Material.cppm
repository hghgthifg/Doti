export module Graphics.Type.Material;

import Core.Math;

export struct Material {
    Vec3  albedo;
    float metallic;
    float roughness;
    float ao;
    float padding1 = 0;
    float padding2 = 0;
};
