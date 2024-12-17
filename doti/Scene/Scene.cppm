export module Scene;

import std;
import Json;
import Core.Event;
import Core.Logger;
import Scene.SceneNode;
import Scene.SceneNodeFactory;
import Graphics.Camera;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Pipeline;

static auto getNameFromJson(const Json& json) -> std::string {
    if (json.is_null()) {
        Logger::error("Empty json object!");
        return "";
    }
    if (json.contains("name")) {
        return json["name"].get<std::string>();
    }
    Logger::error("Cannot find name item!");
    return "";
}

static auto getShaderFileFromJson(const Json& json) -> std::string {
    if (json.is_null()) {
        Logger::error("Empty json object!");
    }
    if (json.contains("shader")) {
        if (json["shader"].is_string()) {
            return json["shader"].get<std::string>();
        }
        Logger::error("Cannot find shader item!");
        return "";
    }
    Logger::error("Cannot find shader item!");
    return "";
}

export class Scene {
public:
    explicit Scene(const std::string& path) {
        if (!_eventsRegistered) {
            _eventsRegistered = true;
        }
        _sceneFilePath = path;
        loadSceneJsonFile(_sceneFilePath);
    }

    auto getName() const -> std::string {
        return getNameFromJson(_sceneJson);
    }

    ~Scene() = default;

    auto load() -> void {
        // TODO: Multiple scenes support
        /* 1. Build the scene from json file. */
        buildSceneTree();

        /* 2. Collect data from drawables and send data to Pipeline. */
        _rootNode->collectData();

        /* 3. Activate current RenderContext. */
        _renderContext = std::make_shared<RenderContext>();
        Pipeline::getInstance().setContext(_renderContext);

        /* 4. Initialize Pipeline */
        Pipeline::getInstance().initialize();
    };

    auto update() -> void {
        EventManager::emit("Render::NewFrame");
        Pipeline::getInstance().render();
    };

    auto exit() -> void {
        _renderContext = nullptr;
        Pipeline::getInstance().clearData();
        saveScene(_sceneFilePath);
        _rootNode = nullptr;
    };

    inline auto setSize(const float width, const float height) -> void {
        if (_width != width || _height != height) {
            _width  = width;
            _height = height;
            EventManager::emit("Scene::Resize", width, height);
        }
    }

private:
    auto loadSceneJsonFile(const std::filesystem::path& path) -> void {
        std::ifstream file(path);
        if (!file.is_open()) {
            Logger::error("Failed to open scene file" + path.filename().string());
            return;
        }
        Json scene_json;
        file >> scene_json;
        this->_sceneJson = std::move(scene_json);
    }

    auto buildSceneTree() -> void {
        if (_sceneJson.is_null()) {
            Logger::error("Empty json object!");
            return;
        }
        if (!_sceneJson.contains("objects")) {
            Logger::error("No objects found in scene node!");
            return;
        }
        _rootNode          = std::static_pointer_cast<CompositeNode>(SceneNodeFactory::createNode(_sceneJson));
        Json scene_objects = _sceneJson["objects"];
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
    std::string                    _sceneFilePath;
    float                          _width = 800, _height = 600;
    Json                           _sceneJson;
    std::shared_ptr<CompositeNode> _rootNode;
    std::shared_ptr<RenderContext> _renderContext;
};

bool Scene::_eventsRegistered = false;

