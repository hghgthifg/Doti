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
export using ::ImGuiViewport;
export using ::ImGuiIO;
export using ::ImTextureID;
export using ::ImGuiID;
export using ::ImU64;
export using ::ImGuiWindowFlags;
export using ::ImGuiDockNodeFlags;

export namespace ImGui
{
    using ImGui::Begin;
    using ImGui::Button;
    using ImGui::CreateContext;
    using ImGui::DestroyContext;
    using ImGui::DestroyPlatformWindows;
    using ImGui::End;
    using ImGui::GetMainViewport;
    using ImGui::GetPlatformIO;
    using ImGui::GetVersion;
    using ImGui::GetWindowViewport;
    using ImGui::BeginChild;
    using ImGui::EndChild;
    using ImGui::BeginMenuBar;
    using ImGui::EndMenuBar;
    using ImGui::BeginMenu;
    using ImGui::MenuItem;
    using ImGui::EndMenu;
    using ImGui::GetContentRegionAvail;
    using ImGui::GetDrawData;
    using ImGui::GetIO;
    using ImGui::GetID;
    using ImGui::Image;
    using ImGui::NewFrame;
    using ImGui::Render;
    using ImGui::SameLine;
    using ImGui::SetCursorPos;
    using ImGui::SetNextWindowBgAlpha;
    using ImGui::SetNextWindowPos;
    using ImGui::SetNextWindowSize;
    using ImGui::SetNextWindowViewport;
    using ImGui::ShowAboutWindow;
    using ImGui::ShowDemoWindow;
    using ImGui::ShowFontSelector;
    using ImGui::ShowMetricsWindow;
    using ImGui::ShowStyleEditor;
    using ImGui::ShowStyleSelector;
    using ImGui::ShowUserGuide;
    using ImGui::StyleColorsDark;
    using ImGui::Text;

    using ImGui::PushStyleVar;
    using ImGui::PopStyleVar;

    using ImGui::SetNextWindowSizeConstraints;

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
    };
    enum class ImGuiStyleVar
    {
        // Enum name -------------------------- // Member in ImGuiStyle structure (see ImGuiStyle for descriptions)
        Alpha,                    // float     Alpha
        DisabledAlpha,            // float     DisabledAlpha
        WindowPadding,            // ImVec2    WindowPadding
        WindowRounding,           // float     WindowRounding
        WindowBorderSize,         // float     WindowBorderSize
        WindowMinSize,            // ImVec2    WindowMinSize
        WindowTitleAlign,         // ImVec2    WindowTitleAlign
        ChildRounding,            // float     ChildRounding
        ChildBorderSize,          // float     ChildBorderSize
        PopupRounding,            // float     PopupRounding
        PopupBorderSize,          // float     PopupBorderSize
        FramePadding,             // ImVec2    FramePadding
        FrameRounding,            // float     FrameRounding
        FrameBorderSize,          // float     FrameBorderSize
        ItemSpacing,              // ImVec2    ItemSpacing
        ItemInnerSpacing,         // ImVec2    ItemInnerSpacing
        IndentSpacing,            // float     IndentSpacing
        CellPadding,              // ImVec2    CellPadding
        ScrollbarSize,            // float     ScrollbarSize
        ScrollbarRounding,        // float     ScrollbarRounding
        GrabMinSize,              // float     GrabMinSize
        GrabRounding,             // float     GrabRounding
        TabRounding,              // float     TabRounding
        TabBorderSize,            // float     TabBorderSize
        TabBarBorderSize,         // float     TabBarBorderSize
        TabBarOverlineSize,       // float     TabBarOverlineSize
        TableAngledHeadersAngle,  // float     TableAngledHeadersAngle
        TableAngledHeadersTextAlign,// ImVec2  TableAngledHeadersTextAlign
        ButtonTextAlign,          // ImVec2    ButtonTextAlign
        SelectableTextAlign,      // ImVec2    SelectableTextAlign
        SeparatorTextBorderSize,  // float     SeparatorTextBorderSize
        SeparatorTextAlign,       // ImVec2    SeparatorTextAlign
        SeparatorTextPadding,     // ImVec2    SeparatorTextPadding
        DockingSeparatorSize,     // float     DockingSeparatorSize
        COUNT
    };
    enum class ImGuiWindowFlags
    {
        None                   = 0,
        NoTitleBar             = 1 << 0,   // Disable title-bar
        NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
        NoMove                 = 1 << 2,   // Disable user moving the window
        NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
        NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
        NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering resource with pass through.
        MenuBar                = 1 << 10,  // Has a menu-bar
        HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        NoNavInputs            = 1 << 16,  // No keyboard/gamepad navigation within the window
        NoNavFocus             = 1 << 17,  // No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by CTRL+TAB)
        UnsavedDocument        = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        NoDocking              = 1 << 19,  // Disable docking of this window
        NoNav                  = NoNavInputs | NoNavFocus,
        NoDecoration           = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
        NoInputs               = NoMouseInputs | NoNavInputs | NoNavFocus,

        // [Internal]
        ChildWindow            = 1 << 24,  // Don't use! For internal use by BeginChild()
        Tooltip                = 1 << 25,  // Don't use! For internal use by BeginTooltip()
        Popup                  = 1 << 26,  // Don't use! For internal use by BeginPopup()
        Modal                  = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
        ChildMenu              = 1 << 28,  // Don't use! For internal use by BeginMenu()
        DockNodeHost           = 1 << 29,  // Don't use! For internal use by Begin()/NewFrame()

        // Obsolete names
    #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        AlwaysUseWindowPadding = 1 << 30,  // Obsoleted in 1.90.0: Use ImGuiChildFlags_AlwaysUseWindowPadding in BeginChild() call.
        NavFlattened           = 1 << 31,  // Obsoleted in 1.90.9: Use ImGuiChildFlags_NavFlattened in BeginChild() call.
    #endif
    };
    enum class ImGuiDockNodeFlags
    {
        None                         = 0,
        KeepAliveOnly                = 1 << 0,   //       // Don't display the dockspace node but keep it alive. Windows docked into this dockspace node won't be undocked.
        //NoCentralNode              = 1 << 1,   //       // Disable Central Node (the node which can stay empty)
        NoDockingOverCentralNode     = 1 << 2,   //       // Disable docking over the Central Node, which will be always kept empty.
        PassthruCentralNode          = 1 << 3,   //       // Enable passthru dockspace: 1) DockSpace() will render a ImGuiCol_WindowBg background covering everything excepted the Central Node when empty. Meaning the host window should probably use SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When Central Node is empty: let inputs pass-through + won't display a DockingEmptyBg background. See demo for details.
        NoDockingSplit               = 1 << 4,   //       // Disable other windows/nodes from splitting this node.
        NoResize                     = 1 << 5,   // Saved // Disable resizing node using the splitter/separators. Useful with programmatically setup dockspaces.
        AutoHideTabBar               = 1 << 6,   //       // Tab bar will automatically hide when there is a single window in the dock node.
        NoUndocking                  = 1 << 7,   //       // Disable undocking this node.

    #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        NoSplit                      = NoDockingSplit, // Renamed in 1.90
        NoDockingInCentralNode       = NoDockingOverCentralNode, // Renamed in 1.90
    #endif
    };

    // clang-format on
} // namespace ImGuiFlags

export namespace ImGui
{
    inline void PushStyleVar(ImGuiFlags::ImGuiStyleVar idx, float val) {
        ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(idx), val);
    }

    inline void PushStyleVar(ImGuiFlags::ImGuiStyleVar idx, ImVec2 val) {
        ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(idx), val);
    }

    inline ImGuiID DockSpace(ImGuiID label, ImVec2 size, ImGuiFlags::ImGuiDockNodeFlags flags) {
        return ImGui::DockSpace(label, size, static_cast<ImGuiDockNodeFlags>(flags));
    }
}

// export using namespace ImGuiFlags;

// To enable bitwise operators for Flag enums in ImGui, use the following code for compatibility.
// Since C++ cannot retrieve type names at compile time, you must manually define them using macros.

template<typename Enum>
struct EnableBitMaskOperators : std::false_type {};

#define ENABLE_BITMASK_OPERATORS(EnumType)                                                                             \
    template<>                                                                                                         \
    struct EnableBitMaskOperators<EnumType> : std::true_type {}

ENABLE_BITMASK_OPERATORS(ImGuiFlags::ImGuiConfigFlags);

ENABLE_BITMASK_OPERATORS(ImGuiFlags::ImGuiStyleVar);

ENABLE_BITMASK_OPERATORS(ImGuiFlags::ImGuiWindowFlags);

ENABLE_BITMASK_OPERATORS(ImGuiFlags::ImGuiDockNodeFlags);

template<typename Enum>
concept ImGuiFlagType = EnableBitMaskOperators<Enum>::value && std::is_enum_v<Enum>;

export template<ImGuiFlagType Enum>
constexpr int operator|(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<int>(static_cast<Underlying>(lhs) | static_cast<Underlying>(rhs));
}

export template<ImGuiFlagType Enum>
constexpr int operator|(int lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return lhs | static_cast<Underlying>(rhs);
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
constexpr int& operator|=(int& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return lhs = lhs | static_cast<Underlying>(rhs);
}

export template<ImGuiFlagType Enum>
constexpr int& operator&=(int& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return lhs = lhs & static_cast<Underlying>(rhs);
}

export template<ImGuiFlagType Enum>
constexpr int operator~(Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return ~static_cast<Underlying>(rhs);
}
