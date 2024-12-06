export module Graphics.Texture;

import std;
import OpenGL;
import Graphics.Render.Drawable.Mesh;

export class ModelTexture {
public:
    ModelTexture() = default;

private:
    GLuint _vertexTexture;

    GLuint _idTexture;
};
