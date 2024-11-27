export module Scene.SceneManager;

import std;
import Debug.Logger;
import Utils.Event;
import Scene.SceneBase;

export class SceneManager {
public:
    static auto initialize() -> void {
        EventManager::connect<std::string>("ChangeScene", [](const std::string& sceneName) {
            setCurrentScene(sceneName);
        });
    }

    static auto registerScene(const std::string& name, std::shared_ptr<SceneBase> scene) -> void {
        getScenes()[name] = std::move(scene);
    }

    static auto getSceneNames() -> const std::vector<std::string>& {
        static std::vector<std::string> names;
        names.clear();
        for (const auto& pair: getScenes()) {
            names.push_back(pair.first);
        }
        return names;
    }

    static auto setCurrentScene(const std::string& name) -> void {
        if (currentScene() != nullptr) {
            currentScene()->exit();
        }
        const auto it = getScenes().find(name);
        if (it != getScenes().end()) {
            currentScene() = it->second;
        } else {
            Logger::error("Scene " + name + " not found.");
        }
    }

    static auto getCurrentScene() -> std::shared_ptr<SceneBase> {
        return currentScene();
    }

private:
    static auto getScenes() -> std::map<std::string, std::shared_ptr<SceneBase>>& {
        static std::map<std::string, std::shared_ptr<SceneBase>> scenes;
        return scenes;
    }

    static auto currentScene() -> std::shared_ptr<SceneBase>& {
        static std::shared_ptr<SceneBase> current = nullptr;
        return current;
    }
};
