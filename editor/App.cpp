import std;
import ImGui;
// import Utils.Event;
import Debug.Logger;
import Scene.SceneManager;
import Scene.SceneBase;
import Scene.FrameCanvas;
import Window;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;
import Window.UI.Components.DockSpace;
import Window.UI.Components.MenuBar;

import Test.TestScene;
import Test.Bunny;

std::shared_ptr<DockSpace> RootComponent;

auto main() -> int {
    try {
        int    width = 1200, height = 800;
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

        while (!window.shouldClose()) {
            window.beginDraw();

            // TODO: TargetTexture should be a component of RenderContext
            targetTexture->bind();

            auto currentScene = SceneManager::getCurrentScene();
            if (currentScene.get() != nullptr) {
                currentScene->setSize(1200, 800);
            }
            if (currentScene) {
                currentScene->render();
            }

            targetTexture->unbind();

            ImGui::NewFrame();
            RootComponent->render();
            ImGui::Render();

            window.endDraw();
        }
    } catch (const std::exception& e) {
        Logger::error(e.what());
        return -1;
    }
    return 0;
}
