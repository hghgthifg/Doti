import std;
import ImGui;
import Debug.Logger;
import Scene.SceneManager;
import Scene.SceneBase;
import Window;
import Window.FrameCanvas;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;
import Window.UI.Components.DockSpace;
import Window.UI.Components.MenuBar;

import Test.TestScene;
import Test.Bunny;

std::shared_ptr<DockSpace> RootComponent;

auto main() -> int {
    try {
        int    width = 1920, height = 1080;
        Window window(width, height, "Doti Engine");

        RootComponent = std::make_shared<DockSpace>("dock_space");

        auto view          = std::make_shared<OpenGLView>("opengl_view");
        auto targetTexture = std::make_shared<FrameCanvas>(width, height);
        auto menu          = std::make_shared<MenuBar>("menu_bar");
        view->setTexture(targetTexture);

        RootComponent->addChild(menu);
        RootComponent->addChild(view);

        Logger::info("Register all UI components. ");

        SceneManager::initialize();

        SceneManager::registerScene("TestScene", std::make_shared<TestScene>());
        // SceneManager::registerScene("BunnyScene", std::make_shared<BunnyScene>());

        while (!window.shouldClose()) {
            window.beginDraw();

            ImGui::NewFrame();
            RootComponent->render();
            ImGui::Render();

            targetTexture->bind();

            auto currentScene = SceneManager::getCurrentScene();
            if (currentScene.get() != nullptr) {
                currentScene->setSize(1920, 1080);
            }
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
