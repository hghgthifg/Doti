export module Graphics.FrameCanvas;

import std;
import OpenGL;
import glfw;
import Core.Logger;
import Core.Event;
import Graphics.Shader;

const std::string MIX_FRAGMENT_SHADER = R"(
#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D rasterTexture;
uniform sampler2D rayTracingAccumTexture;

void main() {
    vec3 rasterColor = texture(rasterTexture, TexCoords).rgb;
    vec3 rayColor = texture(rayTracingAccumTexture, TexCoords).rgb;
    vec3 finalColor = rasterColor + rayColor; // Directly Blend
    FragColor = vec4(finalColor, 1.0);
}
)";

const std::string MIX_VERTEX_SHADER = R"(
#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

export class FrameCanvas {
public:
    FrameCanvas() {
        EventManager::connect<>("Render::RefreshHistoryFramedata", [this] {
            clearRenderResults();
        });
        EventManager::connect<float, float>("Scene::Resize", [this](float width, float height) {
            rescaleFramebuffers(static_cast<int>(width), static_cast<int>(height));
        });
    };

    FrameCanvas(const int32_t width, const int32_t height) : _width(width), _height(height) {
        initializeFrameBuffers();
    }

    ~FrameCanvas() {
        clearRenderResults();
        if (_quadVAO) glDeleteVertexArrays(1, &_quadVAO);
        if (_quadVBO) glDeleteBuffers(1, &_quadVBO);
    }

    auto bindRasterizationFBO() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboRaster);
        glViewport(0, 0, _width, _height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    auto bindRayTracingFBO() -> void {
        // TODO: Switch _currentActiveAccumTexture after raytracing Pass is done.
        if (_currentActiveAccumTexture == 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, _fboRayTracing1);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, _fboRayTracing2);
        }
        glViewport(0, 0, _width, _height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    auto renderToScreen() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboFinal);
        glViewport(0, 0, _width, _height);
        glClear(GL_COLOR_BUFFER_BIT);

        _mixShader.activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _rasterTexture);
        _mixShader.setInt("rasterTexture", 0);

        glActiveTexture(GL_TEXTURE1);
        if (_currentActiveAccumTexture == 0) {
            glBindTexture(GL_TEXTURE_2D, _rayTracingAccumTexture2);
        } else {
            glBindTexture(GL_TEXTURE_2D, _rayTracingAccumTexture1);
        }
        _mixShader.setInt("rayTracingTexture", 1);

        glBindVertexArray(_quadVAO);
        glDrawArrays(GL_TRIANGLES_STRIP, 0, 4);
        glBindVertexArray(0);

        _mixShader.deactivate();
        unbind();
    }

    auto getRasterizationTexture() const -> GLuint { return _rasterTexture; }

    auto getRayTracingAccumTexture() const -> GLuint {
        if (_currentActiveAccumTexture == 0) {
            return _rayTracingAccumTexture2;
        } else {
            return _rayTracingAccumTexture1;
        }
    }

    auto getFinalTexture() const -> GLuint { return _finalTexture; }

    auto rescaleFramebuffers(const int32_t width, const int32_t height) -> void {
        _width  = width;
        _height = height;
        clearRenderResults();
        initializeFrameBuffers();
    }

    auto unbind() -> void { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    auto getWidth() const -> int32_t { return _width; }

    auto getHeight() const -> int32_t { return _height; }

private:
    auto initializeFrameBuffers() -> void {
        /* 1. Generate buffers. */
        glGenFramebuffers(1, &_fboRaster);
        glGenFramebuffers(1, &_fboRayTracing1);
        glGenFramebuffers(1, &_fboRayTracing2);
        glGenFramebuffers(1, &_fboFinal);

        /* 2.1 Generate renderbuffer only for tests during the rasterization pass. */
        glGenRenderbuffers(1, &_rboRasterization);
        glBindRenderbuffer(GL_RENDERBUFFER, _rboRasterization);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        /* 2.2 Generate texture for the rasterization pass. */
        glGenTextures(1, &_rasterTexture);
        glBindTexture(GL_TEXTURE_2D, _rasterTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rasterTexture, 0);

        // Position Texture
        glGenTextures(1, &_gPosition);
        glBindTexture(GL_TEXTURE_2D, _gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);

        // Normal Texture
        glGenTextures(1, &_gNormal);
        glBindTexture(GL_TEXTURE_2D, _gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);

        // AlbedoSpec Texture
        glGenTextures(1, &_gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 或 GL_NEAREST，视需求而定
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 或 GL_NEAREST，视需求而定
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedoSpec, 0);

        /* 2.3 Attach renderbuffer to framebuffer of the rasterization pass. */
        glBindFramebuffer(GL_FRAMEBUFFER, _fboRaster);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rboRasterization);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::error("Rasterization Framebuffer is not complete!");
        }

        /* 3.1 Generate accumulate texture for the rasterization pass. */
        glGenTextures(1, &_rayTracingAccumTexture1);
        glBindTexture(GL_TEXTURE_2D, _rayTracingAccumTexture1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &_rayTracingAccumTexture2);
        glBindTexture(GL_TEXTURE_2D, _rayTracingAccumTexture2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* 3.2 Attach texture to framebuffer of the raytracing pass. */
        glBindFramebuffer(GL_FRAMEBUFFER, _fboRayTracing1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rayTracingAccumTexture1, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::error("Ray Tracing Framebuffer 1 is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, _fboRayTracing2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rayTracingAccumTexture2, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::error("Ray Tracing Framebuffer 2 is not complete!");
        }

        /* 4.1 Generate the final output texture */
        glGenTextures(1, &_finalTexture);
        glBindTexture(GL_TEXTURE_2D, _finalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* 4.2 Attach texture to framebuffer of the final output */
        glBindFramebuffer(GL_FRAMEBUFFER, _fboFinal);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _finalTexture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::error("Final Compositing Framebuffer is not complete!");
        }

        /* 5. Initialize the mixture pass (if not initialized). */
        if (!_mixPassReady) {
            _mixShader = Shader::loadFromString("MixShader", MIX_VERTEX_SHADER, MIX_FRAGMENT_SHADER);
            // clang-format off
            constexpr float quadVertices[] = {
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f ,  1.0f,  1.0f, 1.0f,
                1.0f , -1.0f,  1.0f, 0.0f,
            };
            // clang-format on
            glGenVertexArrays(1, &_quadVAO);
            glGenBuffers(1, &_quadVBO);
            glBindVertexArray(_quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            _mixPassReady = true;
        }
    }

    auto clearRenderResults() -> void {
        if (_fboRaster) glDeleteFramebuffers(1, &_rboRasterization);
        if (_fboRayTracing1) glDeleteFramebuffers(1, &_fboRayTracing1);
        if (_fboRayTracing2) glDeleteFramebuffers(1, &_fboRayTracing2);
        if (_fboFinal) glDeleteFramebuffers(1, &_fboFinal);

        if (_rboRasterization) glDeleteRenderbuffers(1, &_rboRasterization);

        if (_gPosition) glDeleteTextures(1, &_gPosition);
        if (_gNormal) glDeleteTextures(1, &_gNormal);
        if (_gAlbedoSpec) glDeleteTextures(1, &_gAlbedoSpec);

        if (_rasterTexture) glDeleteTextures(1, &_rasterTexture);
        if (_rayTracingAccumTexture1) glDeleteTextures(1, &_rayTracingAccumTexture1);
        if (_rayTracingAccumTexture2) glDeleteTextures(1, &_rayTracingAccumTexture2);
        if (_finalTexture) glDeleteTextures(1, &_finalTexture);
    }

    GLuint _fboRaster      = 0;
    GLuint _fboRayTracing1 = 0;
    GLuint _fboRayTracing2 = 0;
    GLuint _fboFinal       = 0;

    GLuint _rasterTexture           = 0;
    GLuint _rayTracingAccumTexture1 = 0;
    GLuint _rayTracingAccumTexture2 = 0;
    GLuint _finalTexture            = 0;

    GLuint _gPosition   = 0;
    GLuint _gNormal     = 0;
    GLuint _gAlbedoSpec = 0;

    GLuint _rboRasterization = 0;

    int32_t _width                     = 0;
    int32_t _height                    = 0;
    int8_t  _currentActiveAccumTexture = 0;

    GLuint _quadVAO = 0;
    GLuint _quadVBO = 0;

    static Shader _mixShader;
    static bool   _mixPassReady;
};

Shader FrameCanvas::_mixShader    = Shader();
bool   FrameCanvas::_mixPassReady = false;
