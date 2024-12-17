#ifndef COMMON_GLSL
#define COMMON_GLSL

struct Camera {
    vec3 position;
    vec3 front;
    vec3 right;
    vec3 up;
    float halfHeight;
    float halfWidth;
    vec3 leftBottom;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct AreaLight {
    vec3 position;
    vec3 normal;
    vec2 size;
    vec3 color;
    float intensity;
};

struct AABB {
    vec4 pMin;
    vec4 pMax;
};

struct BVHNode {
    AABB box;
    int index; // max size of the tree if > 0, primitive index if < 0
    float _bvh_padding[3];
};

#endif