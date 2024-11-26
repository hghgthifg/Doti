export module Window.UI.Components.OpenGLView;

import ImGui;
import OpenGL;
import std;
import Window.UI.ComponentBase;
import Graphics.Render.RenderTexture;
import Debug.Logger;

export class OpenGLView : public UIComponent {
public:
    OpenGLView(const std::string& name) : UIComponent(name) {}

    auto setTexture(const std::shared_ptr<RenderTexture>& texture) -> void { _texture = texture; }

protected:
    void beginRender() override {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(320, 240),    // 最小尺寸
            ImVec2(10000, 10000) // 最大尺寸（无限制）
        );
        ImGui::Begin(getName().c_str());
    }

    void renderContent() override {
        ImVec2 pos  = ImGui::GetCursorScreenPos();
        ImVec2 size = ImGui::GetContentRegionAvail();

        _texture->rescaleFramebuffer(size.x, size.y);
        // glViewport(0, 0, size.x, size.y);

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID) (intptr_t) (_texture->getTexture()),
            ImVec2(pos.x, pos.y),
            ImVec2(pos.x + size.x, pos.y + size.y),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    }

    void endRender() override {
        ImGui::End();
    }

private:
    std::shared_ptr<RenderTexture> _texture;
    // ImVec2                         _lastSize;
};
