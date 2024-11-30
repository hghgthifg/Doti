export module Scene.SceneBase;

import std;
import Utils.Camera;
import Graphics.Render.RenderContext;

export class SceneBase {
public:
    virtual ~SceneBase() = default;

    virtual void load() = 0;

    virtual void render() = 0;

    virtual void exit() = 0;

    inline auto setSize(const float width, const float height) -> void {
        _width  = width;
        _height = height;
    }

protected:
    Camera        _camera{};
    RenderContext _renderContext{};
    float         _width = 800, _height = 600;
};
