export module Window;

import std;
import glfw;
import ImGui;
import OpenGL;
import Debug.Logger;
import Graphics.Shader;
import Utils.Event;

export class Window {
public:
    Window() = delete;

    Window(const int32_t width, const int32_t height, const char* title) {
        /* Initialize the library */
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW! ");

        /* Glfw Settings */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Logger::info("Initialized OpenGL 4.6. ");

        /* Create the window */
        _window = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>(
            glfwCreateWindow(width, height, title, nullptr, nullptr), [](GLFWwindow* ptr) {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();
                glfwDestroyWindow(ptr);
                glfwTerminate();
            });
        if (!_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window! ");
        }

        int bufferWidth, bufferHeight;
        glfwGetFramebufferSize(_window.get(), &bufferWidth, &bufferHeight);
        glfwMakeContextCurrent(_window.get());
        // glfwSwapInterval(1);

        // glfwSetFramebufferSizeCallback(
        //     _window.get(), [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

        glbinding::initialize(glfwGetProcAddress);

        Logger::info("Created Glfw window. ");

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiFlags::ImGuiConfigFlags::NavEnableKeyboard;
        io.ConfigFlags |= ImGuiFlags::ImGuiConfigFlags::DockingEnable;
        io.ConfigFlags |= ImGuiFlags::ImGuiConfigFlags::ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(_window.get(), true);
        ImGui_ImplOpenGL3_Init("#version 460");

        Logger::info("Inited ImGui. ");

        glEnable(GL_DEPTH_TEST);
    }

    auto beginDraw() -> void {
        /* Render UI */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* Process keyboard and mouse inputs */
        processInput();
    }

    auto endDraw() -> void {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiFlags::ImGuiConfigFlags::ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(_window.get());

        /* Poll for and process events */
        glfwPollEvents();
    }

    auto shouldClose() const -> bool { return glfwWindowShouldClose(_window.get()); }

private:
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> _window;

    auto processInput() const -> void {
        if (glfwGetKey(_window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(_window.get(), true); }
    }
};
