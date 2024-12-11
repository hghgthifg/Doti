module;
#include <imgui.h>
#include <imconfig.h>
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
    using ImGui::BeginTabBar;
    using ImGui::EndTabBar;
    using ImGui::BeginTabItem;
    using ImGui::EndTabItem;
    using ImGui::IsItemActive;
    using ImGui::IsItemClicked;
    using ImGui::IsItemHovered;
    using ImGui::IsItemActivated;
    using ImGui::IsItemDeactivated;
    using ImGui::IsItemFocused;
    using ImGui::IsWindowFocused;
    using ImGui::IsKeyPressed;
    using ImGui::GetContentRegionAvail;
    using ImGui::GetDrawData;
    using ImGui::GetMouseDragDelta;
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
    using ImGui::TextUnformatted;

    using ImGui::PushStyleVar;
    using ImGui::PopStyleVar;
    using ImGui::PushStyleColor;
    using ImGui::PopStyleColor;

    using ImGui::BeginDisabled;

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
    enum class ImGuiFocusedFlags
    {
        None                          = 0,
        ChildWindows                  = 1 << 0,   // Return true if any children of the window is focused
        RootWindow                    = 1 << 1,   // Test from root window (top most parent of the current hierarchy)
        AnyWindow                     = 1 << 2,   // Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
        NoPopupHierarchy              = 1 << 3,   // Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
        DockHierarchy                 = 1 << 4,   // Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
        RootAndChildWindows           = RootWindow | ChildWindows,
    };
    // clang-format on
} // namespace ImGuiFlags

export namespace ImGuiStyles
{
    enum ImGuiCol : int {
        ImGuiCol_Text,
        ImGuiCol_TextDisabled,
        ImGuiCol_WindowBg, // Background of normal windows
        ImGuiCol_ChildBg,  // Background of child windows
        ImGuiCol_PopupBg,  // Background of popups, menus, tooltips windows
        ImGuiCol_Border,
        ImGuiCol_BorderShadow,
        ImGuiCol_FrameBg, // Background of checkbox, radio button, plot, slider, text input
        ImGuiCol_FrameBgHovered,
        ImGuiCol_FrameBgActive,
        ImGuiCol_TitleBg,          // Title bar
        ImGuiCol_TitleBgActive,    // Title bar when focused
        ImGuiCol_TitleBgCollapsed, // Title bar when collapsed
        ImGuiCol_MenuBarBg,
        ImGuiCol_ScrollbarBg,
        ImGuiCol_ScrollbarGrab,
        ImGuiCol_ScrollbarGrabHovered,
        ImGuiCol_ScrollbarGrabActive,
        ImGuiCol_CheckMark, // Checkbox tick and RadioButton circle
        ImGuiCol_SliderGrab,
        ImGuiCol_SliderGrabActive,
        ImGuiCol_Button,
        ImGuiCol_ButtonHovered,
        ImGuiCol_ButtonActive,
        ImGuiCol_Header, // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
        ImGuiCol_HeaderHovered,
        ImGuiCol_HeaderActive,
        ImGuiCol_Separator,
        ImGuiCol_SeparatorHovered,
        ImGuiCol_SeparatorActive,
        ImGuiCol_ResizeGrip, // Resize grip in lower-right and lower-left corners of windows.
        ImGuiCol_ResizeGripHovered,
        ImGuiCol_ResizeGripActive,
        ImGuiCol_TabHovered, // Tab background, when hovered
        ImGuiCol_Tab, // Tab background, when tab-bar is focused & tab is unselected
        ImGuiCol_TabSelected, // Tab background, when tab-bar is focused & tab is selected
        ImGuiCol_TabSelectedOverline, // Tab horizontal overline, when tab-bar is focused & tab is selected
        ImGuiCol_TabDimmed, // Tab background, when tab-bar is unfocused & tab is unselected
        ImGuiCol_TabDimmedSelected, // Tab background, when tab-bar is unfocused & tab is selected
        ImGuiCol_TabDimmedSelectedOverline, //..horizontal overline, when tab-bar is unfocused & tab is selected
        ImGuiCol_DockingPreview, // Preview overlay color when about to docking something
        ImGuiCol_DockingEmptyBg, // Background color for empty node (e.g. CentralNode with no window docked into it)
        ImGuiCol_PlotLines,
        ImGuiCol_PlotLinesHovered,
        ImGuiCol_PlotHistogram,
        ImGuiCol_PlotHistogramHovered,
        ImGuiCol_TableHeaderBg,     // Table header background
        ImGuiCol_TableBorderStrong, // Table outer and header borders (prefer using Alpha=1.0 here)
        ImGuiCol_TableBorderLight,  // Table inner borders (prefer using Alpha=1.0 here)
        ImGuiCol_TableRowBg,        // Table row background (even rows)
        ImGuiCol_TableRowBgAlt,     // Table row background (odd rows)
        ImGuiCol_TextLink,          // Hyperlink color
        ImGuiCol_TextSelectedBg,
        ImGuiCol_DragDropTarget,        // Rectangle highlighting a drop target
        ImGuiCol_NavCursor,             // Color of keyboard/gamepad navigation cursor/rectangle, when visible
        ImGuiCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
        ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
        ImGuiCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
        ImGuiCol_COUNT,

        #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        ImGuiCol_TabActive          = ImGuiCol_TabSelected,       // [renamed in 1.90.9]
        ImGuiCol_TabUnfocused       = ImGuiCol_TabDimmed,         // [renamed in 1.90.9]
        ImGuiCol_TabUnfocusedActive = ImGuiCol_TabDimmedSelected, // [renamed in 1.90.9]
        ImGuiCol_NavHighlight       = ImGuiCol_NavCursor,         // [renamed in 1.91.4]
        #endif
    };
}

export namespace ImGuiInputs
{
    // clang-format off
    enum ImGuiKey : int
    {
        // Keyboard
        ImGuiKey_None = 0,
        ImGuiKey_NamedKey_BEGIN = 512,  // First valid key value (other than 0)

        ImGuiKey_Tab = 512,             // == ImGuiKey_NamedKey_BEGIN
        ImGuiKey_LeftArrow,
        ImGuiKey_RightArrow,
        ImGuiKey_UpArrow,
        ImGuiKey_DownArrow,
        ImGuiKey_PageUp,
        ImGuiKey_PageDown,
        ImGuiKey_Home,
        ImGuiKey_End,
        ImGuiKey_Insert,
        ImGuiKey_Delete,
        ImGuiKey_Backspace,
        ImGuiKey_Space,
        ImGuiKey_Enter,
        ImGuiKey_Escape,
        ImGuiKey_LeftCtrl, ImGuiKey_LeftShift, ImGuiKey_LeftAlt, ImGuiKey_LeftSuper,
        ImGuiKey_RightCtrl, ImGuiKey_RightShift, ImGuiKey_RightAlt, ImGuiKey_RightSuper,
        ImGuiKey_Menu,
        ImGuiKey_0, ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4, ImGuiKey_5, ImGuiKey_6, ImGuiKey_7, ImGuiKey_8, ImGuiKey_9,
        ImGuiKey_A, ImGuiKey_B, ImGuiKey_C, ImGuiKey_D, ImGuiKey_E, ImGuiKey_F, ImGuiKey_G, ImGuiKey_H, ImGuiKey_I, ImGuiKey_J,
        ImGuiKey_K, ImGuiKey_L, ImGuiKey_M, ImGuiKey_N, ImGuiKey_O, ImGuiKey_P, ImGuiKey_Q, ImGuiKey_R, ImGuiKey_S, ImGuiKey_T,
        ImGuiKey_U, ImGuiKey_V, ImGuiKey_W, ImGuiKey_X, ImGuiKey_Y, ImGuiKey_Z,
        ImGuiKey_F1, ImGuiKey_F2, ImGuiKey_F3, ImGuiKey_F4, ImGuiKey_F5, ImGuiKey_F6,
        ImGuiKey_F7, ImGuiKey_F8, ImGuiKey_F9, ImGuiKey_F10, ImGuiKey_F11, ImGuiKey_F12,
        ImGuiKey_F13, ImGuiKey_F14, ImGuiKey_F15, ImGuiKey_F16, ImGuiKey_F17, ImGuiKey_F18,
        ImGuiKey_F19, ImGuiKey_F20, ImGuiKey_F21, ImGuiKey_F22, ImGuiKey_F23, ImGuiKey_F24,
        ImGuiKey_Apostrophe,        // '
        ImGuiKey_Comma,             // ,
        ImGuiKey_Minus,             // -
        ImGuiKey_Period,            // .
        ImGuiKey_Slash,             // /
        ImGuiKey_Semicolon,         // ;
        ImGuiKey_Equal,             // =
        ImGuiKey_LeftBracket,       // [
        ImGuiKey_Backslash,         // \ (this text inhibit multiline comment caused by backslash)
        ImGuiKey_RightBracket,      // ]
        ImGuiKey_GraveAccent,       // `
        ImGuiKey_CapsLock,
        ImGuiKey_ScrollLock,
        ImGuiKey_NumLock,
        ImGuiKey_PrintScreen,
        ImGuiKey_Pause,
        ImGuiKey_Keypad0, ImGuiKey_Keypad1, ImGuiKey_Keypad2, ImGuiKey_Keypad3, ImGuiKey_Keypad4,
        ImGuiKey_Keypad5, ImGuiKey_Keypad6, ImGuiKey_Keypad7, ImGuiKey_Keypad8, ImGuiKey_Keypad9,
        ImGuiKey_KeypadDecimal,
        ImGuiKey_KeypadDivide,
        ImGuiKey_KeypadMultiply,
        ImGuiKey_KeypadSubtract,
        ImGuiKey_KeypadAdd,
        ImGuiKey_KeypadEnter,
        ImGuiKey_KeypadEqual,
        ImGuiKey_AppBack,               // Available on some keyboard/mouses. Often referred as "Browser Back"
        ImGuiKey_AppForward,

        // Gamepad (some of those are analog values, 0.0f to 1.0f)                          // NAVIGATION ACTION
        // (download controller mapping PNG/PSD at http://dearimgui.com/controls_sheets)
        ImGuiKey_GamepadStart,          // Menu (Xbox)      + (Switch)   Start/Options (PS)
        ImGuiKey_GamepadBack,           // View (Xbox)      - (Switch)   Share (PS)
        ImGuiKey_GamepadFaceLeft,       // X (Xbox)         Y (Switch)   Square (PS)        // Tap: Toggle Menu. Hold: Windowing mode (Focus/Move/Resize windows)
        ImGuiKey_GamepadFaceRight,      // B (Xbox)         A (Switch)   Circle (PS)        // Cancel / Close / Exit
        ImGuiKey_GamepadFaceUp,         // Y (Xbox)         X (Switch)   Triangle (PS)      // Text Input / On-screen Keyboard
        ImGuiKey_GamepadFaceDown,       // A (Xbox)         B (Switch)   Cross (PS)         // Activate / Open / Toggle / Tweak
        ImGuiKey_GamepadDpadLeft,       // D-pad Left                                       // Move / Tweak / Resize Window (in Windowing mode)
        ImGuiKey_GamepadDpadRight,      // D-pad Right                                      // Move / Tweak / Resize Window (in Windowing mode)
        ImGuiKey_GamepadDpadUp,         // D-pad Up                                         // Move / Tweak / Resize Window (in Windowing mode)
        ImGuiKey_GamepadDpadDown,       // D-pad Down                                       // Move / Tweak / Resize Window (in Windowing mode)
        ImGuiKey_GamepadL1,             // L Bumper (Xbox)  L (Switch)   L1 (PS)            // Tweak Slower / Focus Previous (in Windowing mode)
        ImGuiKey_GamepadR1,             // R Bumper (Xbox)  R (Switch)   R1 (PS)            // Tweak Faster / Focus Next (in Windowing mode)
        ImGuiKey_GamepadL2,             // L Trig. (Xbox)   ZL (Switch)  L2 (PS) [Analog]
        ImGuiKey_GamepadR2,             // R Trig. (Xbox)   ZR (Switch)  R2 (PS) [Analog]
        ImGuiKey_GamepadL3,             // L Stick (Xbox)   L3 (Switch)  L3 (PS)
        ImGuiKey_GamepadR3,             // R Stick (Xbox)   R3 (Switch)  R3 (PS)
        ImGuiKey_GamepadLStickLeft,     // [Analog]                                         // Move Window (in Windowing mode)
        ImGuiKey_GamepadLStickRight,    // [Analog]                                         // Move Window (in Windowing mode)
        ImGuiKey_GamepadLStickUp,       // [Analog]                                         // Move Window (in Windowing mode)
        ImGuiKey_GamepadLStickDown,     // [Analog]                                         // Move Window (in Windowing mode)
        ImGuiKey_GamepadRStickLeft,     // [Analog]
        ImGuiKey_GamepadRStickRight,    // [Analog]
        ImGuiKey_GamepadRStickUp,       // [Analog]
        ImGuiKey_GamepadRStickDown,     // [Analog]

        // Aliases: Mouse Buttons (auto-submitted from AddMouseButtonEvent() calls)
        // - This is mirroring the data also written to io.MouseDown[], io.MouseWheel, in a format allowing them to be accessed via standard key API.
        ImGuiKey_MouseLeft, ImGuiKey_MouseRight, ImGuiKey_MouseMiddle, ImGuiKey_MouseX1, ImGuiKey_MouseX2, ImGuiKey_MouseWheelX, ImGuiKey_MouseWheelY,

        // [Internal] Reserved for mod storage
        ImGuiKey_ReservedForModCtrl, ImGuiKey_ReservedForModShift, ImGuiKey_ReservedForModAlt, ImGuiKey_ReservedForModSuper,
        ImGuiKey_NamedKey_END,

        // Keyboard Modifiers (explicitly submitted by backend via AddKeyEvent() calls)
        // - This is mirroring the data also written to io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper, in a format allowing
        //   them to be accessed via standard key API, allowing calls such as IsKeyPressed(), IsKeyReleased(), querying duration etc.
        // - Code polling every key (e.g. an interface to detect a key press for input mapping) might want to ignore those
        //   and prefer using the real keys (e.g. ImGuiKey_LeftCtrl, ImGuiKey_RightCtrl instead of ImGuiMod_Ctrl).
        // - In theory the value of keyboard modifiers should be roughly equivalent to a logical or of the equivalent left/right keys.
        //   In practice: it's complicated; mods are often provided from different sources. Keyboard layout, IME, sticky keys and
        //   backends tend to interfere and break that equivalence. The safer decision is to relay that ambiguity down to the end-user...
        // - On macOS, we swap Cmd(Super) and Ctrl keys at the time of the io.AddKeyEvent() call.
        ImGuiMod_None                   = 0,
        ImGuiMod_Ctrl                   = 1 << 12, // Ctrl (non-macOS), Cmd (macOS)
        ImGuiMod_Shift                  = 1 << 13, // Shift
        ImGuiMod_Alt                    = 1 << 14, // Option/Menu
        ImGuiMod_Super                  = 1 << 15, // Windows/Super (non-macOS), Ctrl (macOS)
        ImGuiMod_Mask_                  = 0xF000,  // 4-bits

        // [Internal] If you need to iterate all keys (for e.g. an input mapper) you may use ImGuiKey_NamedKey_BEGIN..ImGuiKey_NamedKey_END.
        ImGuiKey_NamedKey_COUNT         = ImGuiKey_NamedKey_END - ImGuiKey_NamedKey_BEGIN,
        //ImGuiKey_KeysData_SIZE        = ImGuiKey_NamedKey_COUNT,  // Size of KeysData[]: only hold named keys
        //ImGuiKey_KeysData_OFFSET      = ImGuiKey_NamedKey_BEGIN,  // Accesses to io.KeysData[] must use (key - ImGuiKey_NamedKey_BEGIN) index.

    #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        ImGuiKey_COUNT                  = ImGuiKey_NamedKey_END,    // Obsoleted in 1.91.5 because it was extremely misleading (since named keys don't start at 0 anymore)
        ImGuiMod_Shortcut               = ImGuiMod_Ctrl,            // Removed in 1.90.7, you can now simply use ImGuiMod_Ctrl
        ImGuiKey_ModCtrl = ImGuiMod_Ctrl, ImGuiKey_ModShift = ImGuiMod_Shift, ImGuiKey_ModAlt = ImGuiMod_Alt, ImGuiKey_ModSuper = ImGuiMod_Super, // Renamed in 1.89
        //ImGuiKey_KeyPadEnter = ImGuiKey_KeypadEnter,              // Renamed in 1.87
    #endif
    };
    // clang-format on
}

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

    inline bool IsKeyPressed(ImGuiInputs::ImGuiKey key) {
        return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
    }

    inline void PushStyleColor(ImGuiStyles::ImGuiCol col, ImVec4 val) {
        ImGui::PushStyleColor(static_cast<ImGuiCol>(col), val);
    }

    inline bool IsWindowFocused(ImGuiFlags::ImGuiFocusedFlags flags) {
        return ImGui::IsWindowFocused(static_cast<ImGuiFocusedFlags>(flags));
    }

    inline bool BeginChild(const char*                  str_id, const ImVec2& size = ImVec2(0, 0),
                           ImGuiChildFlags              child_flags = 0,
                           ImGuiFlags::ImGuiWindowFlags window_flags = ImGuiFlags::ImGuiWindowFlags::None) {
        return BeginChild(str_id, size, child_flags, static_cast<ImGuiWindowFlags>(window_flags));
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

ENABLE_BITMASK_OPERATORS(ImGuiFlags::ImGuiFocusedFlags);

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
