export module Window.UI.Components.OpenGLView;

import ImGui;
import OpenGL;
import std;
import Window.UI.ComponentBase;
import Window.FrameCanvas;
import Debug.Logger;
import Utils.Event;
import Math;

export class OpenGLView : public UIComponent {
public:
    OpenGLView(const std::string& name) : UIComponent(name) {
        EventManager::registerEvent<Vec2>("MouseDrag");
    }

    auto setTexture(const std::shared_ptr<FrameCanvas>& canvas) -> void { _canvas = canvas; }

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
        const float canvasWidth  = static_cast<float>(_canvas->getWidth());
        const float canvasHeight = static_cast<float>(_canvas->getHeight());

        /* Calculate scaling factor while maintaining aspect ratio
         * Choose the smaller scale to ensure texture fits in the window */
        const float scaleX = availableSize.x / canvasWidth;
        const float scaleY = availableSize.y / canvasHeight;
        const float scale  = std::min(scaleX, scaleY);

        /* Calculate the actual display dimensions after scaling */
        const float displayWidth  = canvasWidth * scale;
        const float displayHeight = canvasHeight * scale;

        /* Calculate offsets to center the texture in available space */
        const float offsetX = (availableSize.x - displayWidth) * 0.5f;
        const float offsetY = (availableSize.y - displayHeight) * 0.5f;

        /* Calculate final display coordinates with centering offsets */
        const ImVec2 displayPos    = ImVec2(pos.x + offsetX, pos.y + offsetY);
        const ImVec2 displayEndPos = ImVec2(displayPos.x + displayWidth, displayPos.y + displayHeight);

        /* Draw the texture maintaining original aspect ratio and centered position
         * UV coordinates are flipped vertically (0,1 to 1,0) to correct OpenGL texture orientation */
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID) (intptr_t) (_canvas->getTexture()),
            displayPos,
            displayEndPos,
            ImVec2(0, 1), // UV start (bottom-left)
            ImVec2(1, 0)  // UV end (top-right)
        );
    }

    void endRender() override {
        if (ImGui::IsItemActive()) {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta();
            if (mouseDelta.x != 0 || mouseDelta.y != 0) {
                Vec2 delta = {mouseDelta.x, mouseDelta.y};
                EventManager::emit("MouseDrag", delta);
            }
        }

        ImGui::End();
    }

private:
    std::shared_ptr<FrameCanvas> _canvas;
    // ImVec2                         _lastSize;
};
