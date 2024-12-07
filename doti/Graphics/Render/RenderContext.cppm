export module Graphics.Render.RenderContext;

import std;
import Math;
import Graphics.Shader;
import Graphics.Camera;
import Debug.Logger;

export class RenderContext;

// template<typename T>
// concept Drawable = requires(T t, RenderContext& ctx)
// {
//     { t.draw(ctx) } -> std::same_as<void>;
// };

export class RenderContext {
public:
    RenderContext() = default;

    ~RenderContext() = default;

    auto apply() -> void {
        if (!_camera.has_value()) {
            Logger::warning("Camera not set! ");
        } else {
            const auto& camera = _camera->get();
            if (!_shader.has_value()) {
                Logger::warning("Shader not set! ");
                return;
            }
            const auto& shader = _shader->get();
            shader.setVec3("camera.position", camera.getCameraPos());
            shader.setVec3("camera.front", camera.getCameraFront());
            shader.setVec3("camera.right", camera.getCameraRight());
            shader.setVec3("camera.up", camera.getCameraUp());
            shader.setFloat("camera.halfHeight", camera.getHalfHeight());
            // Logger::info("camera.halfHeight: " + std::to_string(camera.getHalfHeight()));
            shader.setFloat("camera.halfWidth", camera.getHalfWidth());
            shader.setVec3("camera.leftBottom", camera.getLeftBottomCorner());
        }
        // _shader.setMat4("projection", _projection);
        // _shader.setMat4("view", _view);
        // _shader.setMat4("model", _model);
        // _shader.setVec3("viewPos", _viewPos);
    }

    auto setShader(const Shader& shader) -> void {
        _shader = shader;
    }

    auto setCamera(const Camera& camera) -> void {
        _camera = camera;
    }

    auto getShader() const -> const Shader& {
        return _shader->get();
    }

    // auto setViewMatrix(const Mat4& view) -> void {
    //     _view = view;
    // }

    // auto setProjectionMatrix(const Mat4& projection) -> void {
    //     _projection = projection;
    // }

    // auto setModelMatrix(const Mat4& model) -> void {
    //     _model = model;
    // }

    // auto setViewPos(const Vec3& pos) -> void {
    //     _viewPos = pos;
    // }

    // template<Drawable T>
    //     requires Drawable<T>
    // auto directlyAccessShader(T) -> Shader& {
    //     return _shader;
    // }

private:
    std::optional<std::reference_wrapper<const Shader>> _shader;
    std::optional<std::reference_wrapper<const Camera>> _camera;
    // Mat4                                                _model;
    // Mat4                                                _projection;
    // Mat4                                                _view;
    // Vec3                                                _viewPos;
};
