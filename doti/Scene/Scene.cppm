export module Scene;

import std;
import Json;
import Core.Math;
import Core.Event;
import Core.Logger;
import Scene.SceneNode;
import Scene.SceneNodeFactory;
import Graphics.Camera;
import Graphics.Shader;
import Graphics.Render.RenderContext;
import Graphics.Render.Pipeline;
import Graphics.Type.Material;
import Graphics.Type.Light;

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
        _renderContext->setScreenSize(_width, _height);
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
        if (_sceneJson.contains("materials") && _sceneJson["materials"].is_array()) {
            for (Json material_json: _sceneJson["materials"]) {
                // std::cout << material_json.dump() << "\n";
                Material material{
                    .albedo = Vec3{
                        material_json["albedo"][0].get<float>(),
                        material_json["albedo"][1].get<float>(),
                        material_json["albedo"][2].get<float>()
                    },
                    .metallic = material_json["metallic"].get<float>(),
                    .roughness = material_json["roughness"].get<float>(),
                    .ao = material_json["ao"].get<float>()
                };
                Pipeline::getInstance().addMaterial(material);
            }
        }

        if (_sceneJson.contains("lights") && _sceneJson["lights"].is_array()) {
            auto& pipeline = Pipeline::getInstance();
            for (Json light_json: _sceneJson["lights"]) {
                if (light_json["type"].get<std::string>() == "point") {
                    PointLight pointLight{
                        .position = Vec3{
                            light_json["position"][0].get<float>(),
                            light_json["position"][1].get<float>(),
                            light_json["position"][2].get<float>()
                        },
                        .emission = Vec3{
                            light_json["emission"][0].get<float>(),
                            light_json["emission"][1].get<float>(),
                            light_json["emission"][2].get<float>()
                        }
                    };
                    pipeline.addPointLight(pointLight);
                } else if (light_json["type"].get<std::string>() == "area") {
                    AreaLight areaLight{
                        .position = Vec3{
                            light_json["position"][0].get<float>(),
                            light_json["position"][1].get<float>(),
                            light_json["position"][2].get<float>()
                        },
                        .normal = Vec3{
                            light_json["normal"][0].get<float>(),
                            light_json["normal"][1].get<float>(),
                            light_json["normal"][2].get<float>()
                        },
                        .emission = Vec3{
                            light_json["emission"][0].get<float>(),
                            light_json["emission"][1].get<float>(),
                            light_json["emission"][2].get<float>()
                        },
                        .size = Vec2{
                            light_json["size"][0].get<float>(),
                            light_json["size"][1].get<float>()
                        }
                    };
                    pipeline.addAreaLight(areaLight);
                }
            };
        }
    }

    auto saveScene(const std::filesystem::path& path) -> void {}

    static bool                    _eventsRegistered;
    std::string                    _sceneFilePath;
    float                          _width = 0, _height = 0;
    Json                           _sceneJson;
    std::shared_ptr<CompositeNode> _rootNode;
    std::shared_ptr<RenderContext> _renderContext;
};

bool Scene::_eventsRegistered = false;

