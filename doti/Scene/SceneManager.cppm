export module Scene.SceneManager;

import std;
import Core.Logger;
import Core.Event;
import Scene;

export class SceneManager {
public:
    static auto initialize() -> void {
        EventManager::connect<std::string>("Scene::ChangeScene", [](const std::string& sceneName) {
            setCurrentScene(sceneName);
        });
    }

    static auto registerScene(const std::string& path) -> void {
        auto scene = std::make_shared<Scene>(path);
        if (scene->getName() == "") {
            return;
        }
        getScenes()[scene->getName()] = std::move(scene);
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
            currentScene()->load();
            Logger::info("Loaded scene: " + name);
            EventManager::emit("Render::RefreshHistoryFramedata");
        } else {
            Logger::error("Scene " + name + " not found.");
        }
    }

    static auto getCurrentScene() -> std::shared_ptr<Scene> {
        return currentScene();
    }

private:
    static auto getScenes() -> std::map<std::string, std::shared_ptr<Scene>>& {
        static std::map<std::string, std::shared_ptr<Scene>> scenes;
        return scenes;
    }

    static auto currentScene() -> std::shared_ptr<Scene>& {
        static std::shared_ptr<Scene> current = nullptr;
        return current;
    }
};
