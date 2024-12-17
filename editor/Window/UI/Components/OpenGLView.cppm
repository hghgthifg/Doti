export module Window.UI.Components.OpenGLView;

import ImGui;
import OpenGL;
import std;
import Window.UI.ComponentBase;
import Graphics.FrameCanvas;
import Core.Logger;
import Core.Event;
import Core.Math;

export class OpenGLView : public UIComponent {
public:
    OpenGLView(const std::string& name, float width, float height) : UIComponent(name) {
        EventManager::connect<float, float>("Scene::Resize", [this](float width, float height) {
            _canvasWidth  = width;
            _canvasHeight = height;
        });
        _canvasWidth  = width;
        _canvasHeight = height;
        _open         = new bool(true);
    }

protected:
    void beginRender() override {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(320, 240),
            ImVec2(10000, 10000)
        );
        ImGui::Begin(getName().c_str(), &_visible);
    }

    void renderContent() override {
        /* Get the current cursor position and available space in the window */
        const ImVec2 pos           = ImGui::GetCursorScreenPos();
        const ImVec2 availableSize = ImGui::GetContentRegionAvail();

        /* Calculate scaling factor while maintaining aspect ratio
         * Choose the smaller scale to ensure texture fits in the window */
        const float scaleX = availableSize.x / _canvasWidth;
        const float scaleY = availableSize.y / _canvasHeight;
        const float scale  = std::min(scaleX, scaleY);

        /* Calculate the actual display dimensions after scaling */
        const float displayWidth  = _canvasWidth * scale;
        const float displayHeight = _canvasHeight * scale;

        /* Calculate offsets to center the texture in available space */
        const float offsetX = (availableSize.x - displayWidth) * 0.5f;
        const float offsetY = (availableSize.y - displayHeight) * 0.5f;

        /* Calculate final display coordinates with centering offsets */
        const ImVec2 displayPos    = ImVec2(pos.x + offsetX, pos.y + offsetY);
        const ImVec2 displayEndPos = ImVec2(displayPos.x + displayWidth, displayPos.y + displayHeight);

        /* Draw the texture maintaining original aspect ratio and centered position
         * UV coordinates are flipped vertically (0,1 to 1,0) to correct OpenGL texture orientation */
        ImGui::GetWindowDrawList()->AddImage(
            0,
            // (ImTextureID) (intptr_t) (_canvas->getTexture()),
            displayPos,
            displayEndPos,
            ImVec2(0, 1), // UV start (bottom-left)
            ImVec2(1, 0)  // UV end (top-right)
        );
    }

    void endRender() override {
        ImGuiIO& io     = ImGui::GetIO();
        float    scroll = io.MouseWheel;
        if (scroll != 0.0f && ImGui::IsWindowFocused()) {
            EventManager::emit("Input::MouseScroll", scroll);
        }
        if (ImGui::IsWindowFocused() && !ImGui::IsItemHovered()) {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta();
            if (mouseDelta.x != 0 || mouseDelta.y != 0) {
                float width  = _canvasWidth;
                float height = _canvasHeight;
                Vec2  delta  = {mouseDelta.x / width, mouseDelta.y / height};
                EventManager::emit("Input::MouseDrag", delta);
            }
        }
        if (ImGui::IsKeyPressed(ImGuiInputs::ImGuiKey_W)) {
            EventManager::emit("Camera::MoveForward");
        }
        if (ImGui::IsKeyPressed(ImGuiInputs::ImGuiKey_S)) {
            EventManager::emit("Camera::MoveBackward");
        }
        if (ImGui::IsKeyPressed(ImGuiInputs::ImGuiKey_A)) {
            EventManager::emit("Camera::MoveLeft");
        }
        if (ImGui::IsKeyPressed(ImGuiInputs::ImGuiKey_D)) {
            EventManager::emit("Camera::MoveRight");
        }
        ImGui::End();
    }

private:
    // std::shared_ptr<FrameCanvas> _canvas;
    bool* _open;
    float _canvasWidth, _canvasHeight;
    // ImVec2                         _lastSize;
};
