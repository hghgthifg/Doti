module;
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

export module OpenGL;

export namespace glbinding
{
    using glbinding::initialize;
}

export using gl::ClearBufferMask;
export using gl::glClear;
export constexpr auto GL_COLOR_BUFFER_BIT =  gl::GL_COLOR_BUFFER_BIT;
