#version 460 core
#extension GL_GOOGLE_include_directive: require

#include "common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in uint aMaterialID;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) flat out uint MaterialID;

layout (std140, binding = 0) uniform Matrices
{
    mat4 model;
    mat4 view;
    mat4 projection;
} matrices;

void main()
{
    FragPos = vec3(matrices.model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(matrices.model))) * aNormal;
    MaterialID = aMaterialID;

    gl_Position = matrices.projection * matrices.view * matrices.model * vec4(aPos, 1.0f);
}