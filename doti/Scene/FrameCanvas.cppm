export module Scene.FrameCanvas;

import std;
import OpenGL;
import glfw;
import Debug.Logger;
import Utils.Event;

export class FrameCanvas {
public:
    FrameCanvas() {
        EventManager::connect<>("Render::RefreshHistoryFramedata", [this] {
            Logger::event("Render::RefreshHistoryFramedata");
            rescaleFramebuffers(_width, _height);
        });
    };

    FrameCanvas(const int32_t width, const int32_t height) : _width(width), _height(height) {
        initializeFramebuffers();
    }

    ~FrameCanvas() {
        for (int i = 0; i < 2; ++i) {
            if (_fbo[i]) glDeleteFramebuffers(1, &_fbo[i]);
            if (_rbo[i]) glDeleteRenderbuffers(1, &_rbo[i]);
            if (_texture[i]) glDeleteTextures(1, &_texture[i]);
        }
    }

    auto bind() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo[_currentFBO]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture[_currentFBO ^ 1]);
        glViewport(0, 0, _width, _height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    auto unbind() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Switch to the next FrameBuffer
        _currentFBO = _currentFBO ^ 1;
    }

    auto getTexture() const -> GLuint { return _texture[_currentFBO]; }
    auto getWidth() const -> int32_t { return _width; }
    auto getHeight() const -> int32_t { return _height; }

    auto rescaleFramebuffers(const float width, const float height) -> void {
        _width  = width;
        _height = height;

        for (int i = 0; i < 2; ++i) {
            if (_fbo[i]) glDeleteFramebuffers(1, &_fbo[i]);
            if (_rbo[i]) glDeleteRenderbuffers(1, &_rbo[i]);
            if (_texture[i]) glDeleteTextures(1, &_texture[i]);
        }

        initializeFramebuffers();
    }

private:
    void initializeFramebuffers() {
        glGenFramebuffers(2, _fbo);
        glGenRenderbuffers(2, _rbo);
        glGenTextures(2, _texture);

        // Generate texture

        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, _texture[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);

            // Attach the shared texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture[i], 0);

            // Create render buffer object
            glBindRenderbuffer(GL_RENDERBUFFER, _rbo[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo[i]);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                Logger::error("Framebuffer " + std::to_string(i) + " is not complete!");
            }
        }
        // Unbind framebuffer, texture, and renderbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    GLuint  _rbo[2]     = {0, 0};
    GLuint  _fbo[2]     = {0, 0};
    GLuint  _texture[2] = {0, 0};
    int32_t _width, _height;
    int32_t _currentFBO = 0;
};
