export module Graphics.Render.Object.Types;

import std;
import OpenGL;
import Core.Math;

export typedef uint32_t Index;

export struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoords;
};

export struct Texture {
    GLuint ID;
};
