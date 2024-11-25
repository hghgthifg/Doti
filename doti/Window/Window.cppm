/*!
 * @brief The module which provide windows creation and management functionality.
 */

export module Window;

import std;
import glfw;
import ImGui;
import OpenGL;
import Debug.Logger;
import Graphics.Shader;
import Graphics.Render.RenderTexture;
import Window.UI.ComponentBase;
import Window.UI.Components.OpenGLView;

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

        Logger::info("Initialized OpenGL 4.6.");

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

        glfwSetFramebufferSizeCallback(
            _window.get(), [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

        glbinding::initialize(glfwGetProcAddress);

        Logger::info("Created Glfw window. ");

        ImGui::CreateContext();
        const ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags::NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags::DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags::ViewportsEnable;

        ImGui_ImplGlfw_InitForOpenGL(_window.get(), true);
        ImGui_ImplOpenGL3_Init("#version 460");

        Logger::info("Inited ImGui. ");

        _renderTexture = std::make_shared<RenderTexture>(width / 2, height / 2);
        _rootComponent = std::make_shared<UIComponent>("root");
        _view          = std::make_shared<OpenGLView>("opengl_view");
        _view->setTexture(_renderTexture);
        _rootComponent->addChild(_view);

        Logger::info("Created root component. ");
    }

    auto beginDraw() -> void {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render UI */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        // glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        /* Process keyboard and mouse inputs */
        processInput();

        ImGui::NewFrame();

        _rootComponent->render();
        //
        // ImGui::Begin("view");
        // ImVec2 size = ImGui::GetContentRegionAvail();
        // _renderTexture.rescale_framebuffer(size.x, size.y);
        // glViewport(0, 0, size.x, size.y);
        //
        // ImGui::GetWindowDrawList()->AddImage(
        //     (ImGui)
        // );

        // _rootComponent->render();
        ImGui::Render();

        _renderTexture->bind();
    }

    auto endDraw() -> void {
        _renderTexture->unbind();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);

        /* Swap front and back buffers */
        glfwSwapBuffers(_window.get());
    }

    auto shouldClose() const -> bool { return glfwWindowShouldClose(_window.get()); }

private:
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> _window;
    std::shared_ptr<UIComponent>                                  _rootComponent;
    std::shared_ptr<OpenGLView>                                   _view;
    std::shared_ptr<RenderTexture>                                _renderTexture;

    auto processInput() const -> void {
        if (glfwGetKey(_window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(_window.get(), true); }
    }
};
