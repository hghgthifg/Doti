import std;
import ImGui;
import Debug.Logger;
import Scene.SceneManager;
import Scene.SceneBase;
import Window;
import Window.RenderTexture;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;
import Window.UI.Components.DockSpace;
import Window.UI.Components.MenuBar;

import Test.TestScene;

std::shared_ptr<DockSpace> RootComponent;
// std::shared_ptr<OpenGLView>    View;
// std::shared_ptr<RenderTexture> TargetTexture;

auto main() -> int {
    try {
        int    width = 1920, height = 1080;
        Window window(width, height, "Doti Engine");

        RootComponent = std::make_shared<DockSpace>("dock_space");

        auto view          = std::make_shared<OpenGLView>("opengl_view");
        auto targetTexture = std::make_shared<RenderTexture>(width, height);
        auto menu          = std::make_shared<MenuBar>("menu_bar");
        view->setTexture(targetTexture);

        RootComponent->addChild(menu);
        RootComponent->addChild(view);

        Logger::info("Register all UI components. ");

        SceneManager::initialize();

        SceneManager::registerScene("TestScene", std::make_shared<TestScene>());

        while (!window.shouldClose()) {
            window.beginDraw();

            ImGui::NewFrame();
            RootComponent->render();
            ImGui::Render();

            targetTexture->bind();

            auto currentScene = SceneManager::getCurrentScene();
            if (currentScene) {
                currentScene->render();
            }

            targetTexture->unbind();
            // triangle.draw(shader);
            window.endDraw();
        }
    } catch (const std::exception& e) {
        Logger::error(e.what());
        return -1;
    }
    return 0;
}
