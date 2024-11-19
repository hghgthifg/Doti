module;
#include <glbinding/glbinding.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/gl.h>

export module OpenGL;

export namespace glbinding
{
    using glbinding::initialize;
}

export using gl::ClearBufferMask;
export using gl::glCreateShader;
export using gl::glCompileShader;
export using gl::glShaderSource;
export using gl::glGetShaderiv;
export using gl::glGetProgramiv;
export using gl::glGetProgramInfoLog;
export using gl::glGetShaderInfoLog;
export using gl::glCreateProgram;
export using gl::glAttachShader;
export using gl::glLinkProgram;
export using gl::glDeleteShader;
export using gl::glClear;

export using gl::glUniform1i;
export using gl::glUniform1f;
export using gl::glUniform2fv;
export using gl::glUniform2f;
export using gl::glUniform3fv;
export using gl::glUniform3f;
export using gl::glUniform4fv;
export using gl::glUniform4f;
export using gl::glUniformMatrix2fv;
export using gl::glUniformMatrix3fv;
export using gl::glUniformMatrix4fv;
export using gl::glGetUniformLocation;

export constexpr auto GL_FALSE = gl::GL_FALSE;
export constexpr auto GL_TRUE  = gl::GL_TRUE;

export constexpr auto GL_LINK_STATUS      = gl::GL_LINK_STATUS;
export constexpr auto GL_COMPILE_STATUS   = gl::GL_COMPILE_STATUS;
export constexpr auto GL_VERTEX_SHADER    = gl::GL_VERTEX_SHADER;
export constexpr auto GL_FRAGMENT_SHADER  = gl::GL_FRAGMENT_SHADER;
export constexpr auto GL_COLOR_BUFFER_BIT = gl::GL_COLOR_BUFFER_BIT;
