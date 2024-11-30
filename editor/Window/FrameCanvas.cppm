export module Window.FrameCanvas;

import std;
import OpenGL;
import glfw;
import Debug.Logger;

export class FrameCanvas {
public:
    FrameCanvas() = default;

    FrameCanvas(const int32_t width, const int32_t height) : _width(width), _height(height) {
        initializeFramebuffer();
    }

    ~FrameCanvas() {
        if (_fbo) glDeleteFramebuffers(1, &_fbo);
        if (_texture) glDeleteTextures(1, &_texture);
        if (_rbo) glDeleteRenderbuffers(1, &_rbo);
    }

    auto bind() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glViewport(0, 0, _width, _height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glViewport(0, 0, _width, _height);
    }

    auto unbind() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    auto getTexture() const -> GLuint { return _texture; }
    auto getWidth() const -> int32_t { return _width; }
    auto getHeight() const -> int32_t { return _height; }

    auto rescaleFramebuffer(const float width, const float height) -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::error("Framebuffer is not complete after resize!");
        }

        /* Unbind the objects to get ready for rendering after this */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

private:
    void initializeFramebuffer() {
        /* Create frame buffer object */
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

        /* Generate texture */
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

        /* Create render buffer object */
        glGenRenderbuffers(1, &_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

        /* Check if the fbo is complete */
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::error("Framebuffer is not complete!");
        }

        /* Unbind the objects to get ready for rendering after this */
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint  _rbo     = 0;
    GLuint  _fbo     = 0;
    GLuint  _texture = 0;
    int32_t _width, _height;
};
