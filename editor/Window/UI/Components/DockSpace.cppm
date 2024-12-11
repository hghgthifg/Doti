export module Window.UI.Components.DockSpace;

import std;
import ImGui;
import Window.UI.ComponentBase;

export class DockSpace : public UIComponent {
public:
    DockSpace(const std::string& name) : UIComponent(name) {}

protected:
    void beginRender() override {
        // ImGui::Begin("DockSpace Demo", _open);
        static bool               opt_fullscreen = true;
        static bool               opt_padding    = false;
        static ImGuiDockNodeFlags dockspace_flags;
        dockspace_flags |= ImGuiFlags::ImGuiDockNodeFlags::None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiFlags::ImGuiWindowFlags::MenuBar | ImGuiFlags::ImGuiWindowFlags::NoDocking;
        if (opt_fullscreen) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiFlags::ImGuiStyleVar::WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiFlags::ImGuiStyleVar::WindowBorderSize, 0.0f);
            window_flags |= ImGuiFlags::ImGuiWindowFlags::NoTitleBar | ImGuiFlags::ImGuiWindowFlags::NoCollapse |
                    ImGuiFlags::ImGuiWindowFlags::NoResize |
                    ImGuiFlags::ImGuiWindowFlags::NoMove;
            window_flags |= ImGuiFlags::ImGuiWindowFlags::NoBringToFrontOnFocus |
                    ImGuiFlags::ImGuiWindowFlags::NoNavFocus;
        } else {
            dockspace_flags &= ~ImGuiFlags::ImGuiDockNodeFlags::PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags &
            ImGuiFlags::ImGuiDockNodeFlags::PassthruCentralNode)
            window_flags |= ImGuiFlags::ImGuiWindowFlags::NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding) ImGui::PushStyleVar(ImGuiFlags::ImGuiStyleVar::WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &_visible, window_flags);
        if (!opt_padding) ImGui::PopStyleVar();

        if (opt_fullscreen) ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiFlags::ImGuiConfigFlags::DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("BaseDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
    }

    void renderContent() override {}

    void endRender() override {
        ImGui::End();
    }
};
