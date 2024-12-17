import std;
import ImGui;
import Core.Logger;
import Scene.SceneManager;
import Scene;
import Graphics.FrameCanvas;
import Window;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;
import Window.UI.Components.DockSpace;
import Window.UI.Components.MenuBar;
import Window.UI.Components.Console;

std::shared_ptr<DockSpace> RootComponent;

auto main() -> int {
    try {
        static std::stringstream customLogStream;
        Logger::setOutputStream(customLogStream);

        int    width = 1200, height = 800;
        Window window(width, height, "Doti Engine");

        RootComponent = std::make_shared<DockSpace>("dock_space");

        auto view = std::make_shared<OpenGLView>("opengl_view", width, height);
        // auto targetTexture = std::make_shared<FrameCanvas>(width, height);
        auto console = std::make_shared<Console>("log_console");
        auto menu    = std::make_shared<MenuBar>("menu_bar");

        // view->setTexture(targetTexture);
        console->setLogBuffer(customLogStream);

        RootComponent->addChild(menu);
        RootComponent->addChild(view);
        RootComponent->addChild(console);

        Logger::info("Register all UI components. ");

        SceneManager::initialize();

        SceneManager::registerScene("resource/scenes/cornell-box.json");

        while (!window.shouldClose()) {
            window.beginDraw();

            auto currentScene = SceneManager::getCurrentScene();
            if (currentScene.get() != nullptr) {
                currentScene->setSize(width, height);
            }
            if (currentScene) {
                currentScene->update();
            }

            ImGui::NewFrame();
            RootComponent->render();
            ImGui::Render();

            window.endDraw();
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        return -1;
    }
    Logger::setOutputStream(std::cout);
    return 0;
}
