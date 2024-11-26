import std;
import ImGui;
import Debug.Logger;
import Graphics.Render.RenderTexture;
import Window;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;

std::shared_ptr<UIComponent>   RootComponent;
std::shared_ptr<OpenGLView>    View;
std::shared_ptr<RenderTexture> TargetTexture;

auto main() -> int {
    try {
        int    width = 1920, height = 1080;
        Window window(width, height, "Hello World");
        RootComponent = std::make_shared<UIComponent>("root");
        View          = std::make_shared<OpenGLView>("opengl_view");
        TargetTexture = std::make_shared<RenderTexture>(width, height);
        View->setTexture(TargetTexture);
        RootComponent->addChild(View);

        while (!window.shouldClose()) {
            window.beginDraw();

            ImGui::NewFrame();
            RootComponent->render();
            ImGui::Render();

            TargetTexture->bind();

            TargetTexture->unbind();
            // triangle.draw(shader);
            window.endDraw();
        }
    } catch (const std::exception& e) {
        Logger::error(e.what());
        return -1;
    }
    return 0;
}
