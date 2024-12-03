export module Graphics.Render.RenderContext;

import std;
import Math;
import Graphics.Shader;
import Utils.Camera;
import Utils.Event;
import Utils.Random;
import Debug.Logger;

export class RenderContext;

template<typename T>
concept Drawable = requires(T t, RenderContext& ctx)
{
    { t.draw(ctx) } -> std::same_as<void>;
};

export class RenderContext {
public:
    RenderContext() = default;

    ~RenderContext() = default;

    auto apply() -> void {
        if (!_camera.has_value()) {
            Logger::warning("Camera not set! ");
        } else {
            const auto& camera = _camera->get();
            _shader.setInt("historyTexture", 0);
            _shader.setVec3("camera.position", camera.getCameraPos());
            _shader.setVec3("camera.front", camera.getCameraFront());
            _shader.setVec3("camera.right", camera.getCameraRight());
            _shader.setVec3("camera.up", camera.getCameraUp());
            _shader.setFloat("camera.halfHeight", camera.getHalfHeight());
            _shader.setFloat("camera.halfWidth", camera.getHalfWidth());
            _shader.setVec3("camera.leftBottom", camera.getLeftBottomCorner());
            _shader.setInt("frameCount", _frameCount);
            _shader.setFloat("randOrigin", 674764.0f * (Random::randFloat() + 1.0f));
        }
    }

    auto setShader(Shader&& shader) -> void {
        _shader = std::move(shader);
    }

    auto setCamera(const Camera& camera) -> void {
        _camera = camera;
    }

    auto setFrameCount(int32_t count) -> void {
        _frameCount = count;
    }

    // auto setViewMatrix(const Mat4& view) -> void {
    //     _view = view;
    // }
    //
    // auto setProjectionMatrix(const Mat4& projection) -> void {
    //     _projection = projection;
    // }
    //
    // auto setModelMatrix(const Mat4& model) -> void {
    //     _model = model;
    // }
    //
    // auto setViewPos(const Vec3& pos) -> void {
    //     _viewPos = pos;
    // }

    template<Drawable T>
        requires Drawable<T>
    auto directlyAccessShader(T) -> Shader& {
        return _shader;
    }

private:
    Shader                                              _shader{};
    std::optional<std::reference_wrapper<const Camera>> _camera{};
    int32_t                                             _frameCount = 0;
    // Mat4 _projection;
    // Mat4 _view;
    // Mat4 _model;
    // Vec3 _viewPos;
};
