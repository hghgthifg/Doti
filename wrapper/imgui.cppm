module;
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

export module ImGui;

import std;

export using ::ImGui_ImplGlfw_InitForOpenGL;
export using ::ImGui_ImplOpenGL3_Init;
export using ::ImGui_ImplOpenGL3_NewFrame;
export using ::ImGui_ImplGlfw_NewFrame;
export using ::ImGui_ImplOpenGL3_RenderDrawData;
export using ::ImGui_ImplOpenGL3_Shutdown;
export using ::ImGui_ImplGlfw_Shutdown;

export using ::ImVec2;
export using ::ImVec4;
export using ::ImColor;
export using ::ImTextureID;
export using ::ImU64;

export using ::ImGuiIO;

export namespace ImGui
{
    using ImGui::Begin;
    using ImGui::Button;
    using ImGui::CreateContext;
    using ImGui::DestroyContext;
    using ImGui::DestroyPlatformWindows;
    using ImGui::End;
    using ImGui::GetContentRegionAvail;
    using ImGui::GetDrawData;
    using ImGui::GetIO;
    using ImGui::Image;
    using ImGui::NewFrame;
    using ImGui::Render;
    using ImGui::SameLine;
    using ImGui::SetCursorPos;
    using ImGui::SetNextWindowBgAlpha;
    using ImGui::SetNextWindowPos;
    using ImGui::SetNextWindowSize;
    using ImGui::ShowAboutWindow;
    using ImGui::ShowDemoWindow;
    using ImGui::ShowFontSelector;
    using ImGui::ShowMetricsWindow;
    using ImGui::ShowStyleEditor;
    using ImGui::ShowStyleSelector;
    using ImGui::ShowUserGuide;
    using ImGui::Text;

    using ImGui::UpdatePlatformWindows;
    using ImGui::RenderPlatformWindowsDefault;

    using ImGui::GetCursorScreenPos;
    using ImGui::GetWindowDrawList;

    using ImGui::DockSpace;
    using ImGui::DockSpaceOverViewport;
    using ImGui::GetMainViewport;
} // namespace ImGui

export namespace ImGuiFlags
{
    // clang-format off
    enum class ImGuiConfigFlags {
        None                   = 0,
        NavEnableKeyboard      = 1 << 0,   // Master keyboard navigation enable flag. Enable full Tabbing + directional arrows + space/enter to activate.
        NavEnableGamepad       = 1 << 1,   // Master gamepad navigation enable flag. Backend also needs to set ImGuiBackendFlags_HasGamepad.
        NoMouse                = 1 << 4,   // Instruct dear imgui to disable mouse inputs and interactions.
        NoMouseCursorChange    = 1 << 5,   // Instruct backend to not alter mouse cursor shape and visibility. Use if the backend cursor changes are interfering with yours and you don't want to use SetMouseCursor() to change mouse cursor. You may want to honor requests from imgui by reading GetMouseCursor() yourself instead.
        NoKeyboard             = 1 << 6,   // Instruct dear imgui to disable keyboard inputs and interactions. This is done by ignoring keyboard events and clearing existing states.

        // [BETA] Docking
        DockingEnable          = 1 << 7,   // Docking enable flags.

        // [BETA] Viewports
        // When using viewports it is recommended that your default value for ImGuiCol_WindowBg is opaque (Alpha=1.0) so transition to a viewport won't be noticeable.
        ViewportsEnable        = 1 << 10,  // Viewport enable flags (require both ImGuiBackendFlags_PlatformHasViewports + ImGuiBackendFlags_RendererHasViewports set by the respective backends)
        DpiEnableScaleViewports= 1 << 14,  // [BETA: Don't use] FIXME-DPI: Reposition and resize imgui windows when the DpiScale of a viewport changed (mostly useful for the main viewport hosting other window). Note that resizing the main window itself is up to your application.
        DpiEnableScaleFonts    = 1 << 15,  // [BETA: Don't use] FIXME-DPI: Request bitmap-scaled fonts to match DpiScale. This is a very low-quality workaround. The correct way to handle DPI is _currently_ to replace the atlas and/or fonts in the Platform_OnChangedViewport callback, but this is all early work in progress.

        // User storage (to allow your backend/engine to communicate to code that may be shared between multiple projects. Those flags are NOT used by core Dear ImGui)
        IsSRGB                 = 1 << 20,  // Application is SRGB-aware.
        IsTouchScreen          = 1 << 21,  // Application is using a touch screen instead of a mouse.

    #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        NavEnableSetMousePos   = 1 << 2,   // [moved/renamed in 1.91.4] -> use bool io.ConfigNavMoveSetMousePos
        NavNoCaptureKeyboard   = 1 << 3,   // [moved/renamed in 1.91.4] -> use bool io.ConfigNavCaptureKeyboard
    #endif
    };;
    // clang-format on
} // namespace ImGuiFlags

export using namespace ImGuiFlags;

// To enable bitwise operators for Flag enums in ImGui, use the following code for compatibility.
// Since C++ cannot retrieve type names at compile time, you must manually define them using macros.

template<typename Enum>
struct EnableBitMaskOperators : std::false_type {};

#define ENABLE_BITMASK_OPERATORS(EnumType)                                                                             \
    template<>                                                                                                         \
    struct EnableBitMaskOperators<EnumType> : std::true_type {}

ENABLE_BITMASK_OPERATORS(ImGuiFlags::ImGuiConfigFlags);

template<typename Enum>
concept ImGuiFlagType = EnableBitMaskOperators<Enum>::value && std::is_enum_v<Enum>;

export template<ImGuiFlagType Enum>
constexpr int operator|(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<int>(static_cast<Underlying>(lhs) | static_cast<Underlying>(rhs));
}

export template<ImGuiFlagType Enum>
constexpr int operator&(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<int>(static_cast<Underlying>(lhs) & static_cast<Underlying>(rhs));
}

export template<ImGuiFlagType Enum>
constexpr int operator&(int lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<int>(static_cast<Underlying>(lhs) & static_cast<Underlying>(rhs));
}

export template<ImGuiFlagType Enum>
constexpr int operator|=(int lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return lhs | static_cast<Underlying>(rhs);
}

export template<ImGuiFlagType Enum>
constexpr int operator&=(int lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return lhs & static_cast<Underlying>(rhs);
}
