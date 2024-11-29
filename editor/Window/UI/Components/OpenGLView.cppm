export module Window.UI.Components.OpenGLView;

import ImGui;
import OpenGL;
import std;
import Window.UI.ComponentBase;
import Window.RenderTexture;
import Debug.Logger;

export class OpenGLView : public UIComponent {
public:
    OpenGLView(const std::string& name) : UIComponent(name) {}

    auto setTexture(const std::shared_ptr<RenderTexture>& texture) -> void { _texture = texture; }

protected:
    void beginRender() override {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(320, 240),
            ImVec2(10000, 10000)
        );
        ImGui::Begin(getName().c_str());
    }

    void renderContent() override {
        /* Get the current cursor position and available space in the window */
        const ImVec2 pos           = ImGui::GetCursorScreenPos();
        const ImVec2 availableSize = ImGui::GetContentRegionAvail();

        /* Get the original texture dimensions */
        const float textureWidth  = static_cast<float>(_texture->getWidth());
        const float textureHeight = static_cast<float>(_texture->getHeight());

        /* Calculate scaling factor while maintaining aspect ratio
         * Choose the smaller scale to ensure texture fits in the window */
        const float scaleX = availableSize.x / textureWidth;
        const float scaleY = availableSize.y / textureHeight;
        const float scale  = std::min(scaleX, scaleY);

        /* Calculate the actual display dimensions after scaling */
        const float displayWidth  = textureWidth * scale;
        const float displayHeight = textureHeight * scale;

        /* Calculate offsets to center the texture in available space */
        const float offsetX = (availableSize.x - displayWidth) * 0.5f;
        const float offsetY = (availableSize.y - displayHeight) * 0.5f;

        /* Calculate final display coordinates with centering offsets */
        const ImVec2 displayPos    = ImVec2(pos.x + offsetX, pos.y + offsetY);
        const ImVec2 displayEndPos = ImVec2(displayPos.x + displayWidth, displayPos.y + displayHeight);

        /* Draw the texture maintaining original aspect ratio and centered position
         * UV coordinates are flipped vertically (0,1 to 1,0) to correct OpenGL texture orientation */
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID) (intptr_t) (_texture->getTexture()),
            displayPos,
            displayEndPos,
            ImVec2(0, 1), // UV start (bottom-left)
            ImVec2(1, 0)  // UV end (top-right)
        );
    }

    void endRender() override {
        ImGui::End();
    }

private:
    std::shared_ptr<RenderTexture> _texture;
    // ImVec2                         _lastSize;
};
