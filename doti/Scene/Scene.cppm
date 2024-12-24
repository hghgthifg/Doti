export module Scene;

import std;
import Json;
import Core.Event;
import Core.Math;
import Core.Logger;
import Scene.SceneNode;
import Scene.SceneNodeFactory;
import Graphics.Camera;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Canvas;
import Graphics.Render.Acceleration.BVHTree;

export class Scene {
public:
    explicit Scene(const std::string& path) {
        if (!_eventsRegistered) {
            EventManager::registerEvent<>("Render::RefreshHistoryFramedata");
            EventManager::registerEvent<>("Scene::NewRenderLoop");
            _eventsRegistered = true;
        }
        _sceneFilePath = path;
        loadScene(_sceneFilePath);
    }

    auto getName() -> std::string {
        return _name;
    }

    ~Scene() = default;

    auto load() -> void {
        // TODO: Multiple scenes support
        bindEvents();
        _rootNode->load(_renderContext);
        _renderContext.setCamera(_camera);
        // std::cout << _renderLoopCount << "\n";
        _renderContext.setFrameCount(_renderLoopCount);
        _renderContext.setup();
    };

    auto render() -> void {
        EventManager::emit("Scene::NewRenderLoop");
        _canvas.draw(_renderContext);
    };

    auto exit() -> void {
        unbindEvents();
        _renderContext = RenderContext();
        saveScene(_sceneFilePath);
        _rootNode = nullptr;
    };

    inline auto setSize(const float width, const float height) -> void {
        _width  = width;
        _height = height;
        _camera.updateScreenRatio(_width, _height);
    }

protected:
    auto bindEvents() -> void {
        EventManager::connect<Vec2>("Input::MouseDrag", [this](const Vec2& delta) {
            _camera.updateOrientation(delta.x, delta.y);
            EventManager::emit("Render::RefreshHistoryFramedata");
        });
        EventManager::connect<float>("Input::MouseScroll", [this](float delta) {
            const auto fov = _camera.getFov();
            _camera.setFov(Math::clamp(fov - delta, 0.1f, 80.0f));
            EventManager::emit("Render::RefreshHistoryFramedata");
        });
        EventManager::connect<>("Camera::MoveLeft", [this]() {
            this->_camera.moveLeft();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Camera::MoveRight", [this]() {
            this->_camera.moveRight();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Camera::MoveForward", [this]() {
            this->_camera.moveForward();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Camera::MoveBackward", [this]() {
            this->_camera.moveBackward();
            EventManager::emit("Camera::Move");
        });
        EventManager::connect<>("Scene::NewRenderLoop", [this] {
            this->_renderLoopCount = this->_renderLoopCount + 1;
        });
        EventManager::connect<>("Render::RefreshHistoryFramedata", [this] {
            this->_renderLoopCount = 0;
        });
    }

    auto unbindEvents() -> void {
        EventManager::disconnectAll<Vec2>("Input::MouseDrag");
        EventManager::disconnectAll<float>("Input::MouseScroll");
        EventManager::disconnectAll<>("Camera::MoveLeft");
        EventManager::disconnectAll<>("Camera::MoveRight");
        EventManager::disconnectAll<>("Camera::MoveForward");
        EventManager::disconnectAll<>("Camera::MoveBackward");
        EventManager::disconnectAll<>("Scene::NewRenderLoop");
        EventManager::disconnectAll<>("Render::RefreshHistoryFramedata");
    }

    auto loadScene(const std::filesystem::path& path) -> void {
        std::ifstream file(path);
        if (!file.is_open()) {
            Logger::info("Failed to open scene file" + path.filename().string());
            return;
        }
        Json scene_json;
        file >> scene_json;
        this->parseSceneJson(scene_json);
    }

    auto parseSceneJson(const Json& scene_json) -> void {
        if (scene_json.is_null()) {
            Logger::error("Failed to parse scene file!");
            return;
        }
        if (!scene_json.contains("scene")) {
            Logger::error("Cannot find scene node!");
            return;
        }
        Json scene_node = scene_json["scene"];
        if (scene_node.contains("name")) {
            _name = scene_node["name"].get<std::string>();
        } else {
            Logger::error("Cannot find scene name!");
            return;
        }

        if (!scene_node.contains("shader")) {
            Logger::error("Cannot find scene shader!");
            return;
        }
        Json shader_node = scene_node["shader"];
        if (!shader_node.contains("fragment_shader") || !shader_node.contains("fragment_shader")) {
            Logger::error("Shader incomplete!");
        }
        auto   frag_path = shader_node["fragment_shader"].get<std::string>();
        auto   vert_path = shader_node["vertex_shader"].get<std::string>();
        Shader shader(_name, vert_path, frag_path);
        _renderContext.setShader(std::move(shader));

        if (!scene_node.contains("objects")) {
            Logger::warning("No objects found in scene node!");
        }
        _rootNode = std::static_pointer_cast<CompositeNode>(SceneNodeFactory::createNode(scene_node));

        Json scene_objects = scene_node["objects"];
        if (!scene_objects.is_array()) {
            Logger::error("scene_objects is not an array!");
            return;
        }
        for (Json object: scene_objects) {
            if (!object.is_object()) {
                Logger::error("Object format error!");
                continue;
            }
            if (!object.contains("type")) {
                Logger::error("Object type unknown!");
                continue;
            }
            _rootNode->addChild(SceneNodeFactory::createNode(object));
        }
    }

    auto saveScene(const std::filesystem::path& path) -> void {}

    static bool                    _eventsRegistered;
    std::string                    _name;
    std::string                    _sceneFilePath;
    int32_t                        _renderLoopCount = 0;
    float                          _width           = 800, _height = 600;
    Camera                         _camera{_width, _height};
    Canvas                         _canvas;
    RenderContext                  _renderContext{};
    std::shared_ptr<CompositeNode> _rootNode;
};

bool Scene::_eventsRegistered = false;

