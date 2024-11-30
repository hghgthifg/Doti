export module Graphics.Render.RenderContext;

import std;
import Math;
import Graphics.Shader;
import Utils.Camera;

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
        _shader.setMat4("projection", _projection);
        _shader.setMat4("view", _view);
        _shader.setMat4("model", _model);
        _shader.setVec3("viewPos", _viewPos);
    }

    auto setShader(Shader&& shader) -> void {
        _shader = std::move(shader);
    }

    auto setViewMatrix(const Mat4& view) -> void {
        _view = view;
    }

    auto setProjectionMatrix(const Mat4& projection) -> void {
        _projection = projection;
    }

    auto setModelMatrix(const Mat4& model) -> void {
        _model = model;
    }

    auto setViewPos(const Vec3& pos) -> void {
        _viewPos = pos;
    }

    template<Drawable T>
        requires Drawable<T>
    auto directlyAccessShader(T) -> Shader& {
        return _shader;
    }

private:
    Shader _shader;

    Mat4 _projection;
    Mat4 _view;
    Mat4 _model;
    Vec3 _viewPos;
};
