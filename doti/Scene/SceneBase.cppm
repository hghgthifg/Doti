export module Scene.SceneBase;

import std;
import Utils.Event;
import Utils.Camera;
import Debug.Logger;
import Graphics.Render.RenderContext;

export class SceneBase {
public:
    SceneBase() {
        if (!_eventsRegistered) {
            EventManager::registerEvent<>("Render::RefreshHistoryFramedata");
            EventManager::registerEvent<>("Scene::NewRenderLoop");
            EventManager::connect<>("Scene::NewRenderLoop", [this] {
                this->_renderLoopCount = this->_renderLoopCount + 1;
            });
            EventManager::connect<>("Render::RefreshHistoryFramedata", [this] {
                this->_renderLoopCount = 0;
            });
            _eventsRegistered = true;
        }
    }

    virtual ~SceneBase() = default;

    virtual void load() = 0;

    virtual void render() = 0;

    virtual void exit() = 0;

    inline auto setSize(const float width, const float height) -> void {
        _width  = width;
        _height = height;
        _camera.updateScreenRatio(_width, _height);
    }

protected:
    static bool   _eventsRegistered;
    int32_t       _renderLoopCount = 0;
    float         _width           = 800, _height = 600;
    Camera        _camera{_width, _height};
    RenderContext _renderContext{};
};

bool SceneBase::_eventsRegistered = false;
