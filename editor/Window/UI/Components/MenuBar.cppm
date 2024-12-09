export module Window.UI.Components.MenuBar;

import std;
import ImGui;
import Utils.Event;
import Window.UI.ComponentBase;
import Utils.Event;
import Scene.SceneManager;

export class MenuBar : public UIComponent {
public:
    MenuBar(const std::string& name) : UIComponent(name) {
        EventManager::registerEvent<std::string>("Scene::ChangeScene");
    }

protected:
    void beginRender() override {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Scenes")) {
                const auto& sceneNames = SceneManager::getSceneNames();
                for (const auto& name: sceneNames) {
                    if (ImGui::MenuItem(name.c_str())) {
                        EventManager::emit("Scene::ChangeScene", name);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void renderContent() override {}

    void endRender() override {}
};
