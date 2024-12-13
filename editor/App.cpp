import std;
import ImGui;
// import Core.Event;
import Core.Logger;
import Scene.SceneManager;
import Scene.SceneBase;
import Graphics.FrameCanvas;
import Window;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;
import Window.UI.Components.DockSpace;
import Window.UI.Components.MenuBar;
import Window.UI.Components.Console;

import Test.TestScene;
import Test.BallSpace;

std::shared_ptr<DockSpace> RootComponent;

auto main() -> int {
    try {
        static std::stringstream customLogStream;
        Logger::setOutputStream(customLogStream);

        int    width = 1200, height = 800;
        Window window(width, height, "Doti Engine");

        RootComponent = std::make_shared<DockSpace>("dock_space");

        auto view          = std::make_shared<OpenGLView>("opengl_view");
        auto targetTexture = std::make_shared<FrameCanvas>(width, height);
        auto console       = std::make_shared<Console>("log_console");
        auto menu          = std::make_shared<MenuBar>("menu_bar");
        view->setTexture(targetTexture);
        console->setLogBuffer(customLogStream);

        RootComponent->addChild(menu);
        RootComponent->addChild(view);
        RootComponent->addChild(console);

        Logger::info("Register all UI components. ");

        SceneManager::initialize();

        SceneManager::registerScene("BallSpace", std::make_shared<BallSpace>());
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
