#version 460 core
#extension GL_GOOGLE_include_directive: require

#include "common.glsl"

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) flat in uint MaterialID;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoMetallic;
layout (location = 3) out vec2 gRoughnessAO;

layout (std140, binding = 1) uniform Materials {
    Material materials[100];
};

void main() {
    gPosition = FragPos;
    gNormal = normalize(Normal);

    Material material = materials[MaterialID];

    gAlbedoMetallic.rgb = material.albedo;
    gAlbedoMetallic.w = material.metallic;

    gRoughnessAO.r = material.roughness;
    gRoughnessAO.g = material.ao;
}
