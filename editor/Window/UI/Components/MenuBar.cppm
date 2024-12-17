export module Window.UI.Components.MenuBar;

import std;
import ImGui;
import Core.Event;
import Window.UI.ComponentBase;
import Core.Event;
import Scene.SceneManager;

export class MenuBar : public UIComponent {
public:
    MenuBar(const std::string& name) : UIComponent(name) {}

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
