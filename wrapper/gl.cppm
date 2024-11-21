module;
#include <glbinding/glbinding.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/gl.h>

export module OpenGL;

export namespace glbinding
{
    using glbinding::initialize;
}

export using gl::GLfloat;
export using gl::GLuint;
export using gl::GLint;
export using gl::GLsizei;
export using gl::GLboolean;
export using gl::GLchar;
export using gl::GLenum;

export using gl::ClearBufferMask;

export using gl::glViewport;

export using gl::glUseProgram;
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

export using gl::glGenVertexArrays;
export using gl::glGenBuffers;
export using gl::glBindVertexArray;
export using gl::glBindBuffer;
export using gl::glBufferData;

export using gl::glBindTexture;
export using gl::glActiveTexture;

export using gl::glDrawElements;
export using gl::glDrawArrays;

export using gl::glVertexAttribPointer;
export using gl::glEnableVertexAttribArray;

export constexpr auto GL_BYTE               = gl::GL_BYTE;
export constexpr auto GL_UNSIGNED_SHORT     = gl::GL_UNSIGNED_SHORT;
export constexpr auto GL_FLOAT              = gl::GL_FLOAT;
export constexpr auto GL_UNSIGNED_INT       = gl::GL_UNSIGNED_INT;
export constexpr auto GL_UNSIGNED_BYTE      = gl::GL_UNSIGNED_BYTE;
export constexpr auto GL_RGB                = gl::GL_RGB;
export constexpr auto GL_RGBA               = gl::GL_RGBA;
export constexpr auto GL_TRIANGLES          = gl::GL_TRIANGLES;
export constexpr auto GL_TEXTURE_2D         = gl::GL_TEXTURE_2D;
export constexpr auto GL_TEXTURE0           = gl::GL_TEXTURE0;
export constexpr auto GL_TEXTURE1           = gl::GL_TEXTURE1;
export constexpr auto GL_TEXTURE2           = gl::GL_TEXTURE2;
export constexpr auto GL_TEXTURE3           = gl::GL_TEXTURE3;
export constexpr auto GL_TEXTURE_WRAP_S     = gl::GL_TEXTURE_WRAP_S;
export constexpr auto GL_TEXTURE_WRAP_T     = gl::GL_TEXTURE_WRAP_T;
export constexpr auto GL_TEXTURE_MIN_FILTER = gl::GL_TEXTURE_MIN_FILTER;
export constexpr auto GL_TEXTURE_MAG_FILTER = gl::GL_TEXTURE_MAG_FILTER;
export constexpr auto GL_NEAREST            = gl::GL_NEAREST;
export constexpr auto GL_LINEAR             = gl::GL_LINEAR;
export constexpr auto GL_REPEAT             = gl::GL_REPEAT;
export constexpr auto GL_CLAMP_TO_EDGE      = gl::GL_CLAMP_TO_EDGE;
export constexpr auto GL_MIRRORED_REPEAT    = gl::GL_MIRRORED_REPEAT;

export constexpr auto GL_FALSE = gl::GL_FALSE;
export constexpr auto GL_TRUE  = gl::GL_TRUE;

export constexpr auto GL_ARRAY_BUFFER         = gl::GL_ARRAY_BUFFER;
export constexpr auto GL_ELEMENT_ARRAY_BUFFER = gl::GL_ELEMENT_ARRAY_BUFFER;

export constexpr auto GL_STATIC_DRAW = gl::GL_STATIC_DRAW;

export constexpr auto GL_LINK_STATUS      = gl::GL_LINK_STATUS;
export constexpr auto GL_COMPILE_STATUS   = gl::GL_COMPILE_STATUS;
export constexpr auto GL_VERTEX_SHADER    = gl::GL_VERTEX_SHADER;
export constexpr auto GL_FRAGMENT_SHADER  = gl::GL_FRAGMENT_SHADER;
export constexpr auto GL_COLOR_BUFFER_BIT = gl::GL_COLOR_BUFFER_BIT;