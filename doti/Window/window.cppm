/*!
 * @brief The module which provide windows creation and management functionality.
 */

export module Window;

import std;
import glfw;
import OpenGL;
import Debug.Logger;
import Graphics.Shader;

export class Window {
public:
    Window() = delete;

    Window(const int32_t width, const int32_t height, const char* title) {
        /* Initialize the library */
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

        /* Specify OpenGL 4.6 */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        Logger::info("Successfully initialize OpenGL 4.6.");

        /* Create the window */
        _window = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>(
            glfwCreateWindow(width, height, title, nullptr, nullptr),
            [](GLFWwindow* ptr) {
                glfwDestroyWindow(ptr);
                glfwTerminate();
            }
        );
        if (!_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(_window.get());
        glfwSetFramebufferSizeCallback(_window.get(), [](GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);
        });

        glbinding::initialize(glfwGetProcAddress);

        Logger::info("Successfully create OpenGL window.");
    }

    // auto loop() const -> void {
    //     /* Loop until the user closes the window */
    //     while (!glfwWindowShouldClose(_window.get())) {
    //         /* Render here */
    //         glClear(GL_COLOR_BUFFER_BIT);
    //
    //         /* Process keyboard and mouse inputs */
    //         processInput();
    //
    //         /* Swap front and back buffers */
    //         glfwSwapBuffers(_window.get());
    //
    //         /* Poll for and process events */
    //         glfwPollEvents();
    //     }
    // }

    auto shouldClose() const -> bool {
        return glfwWindowShouldClose(_window.get());
    }

    auto beginDraw() const -> void {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Process keyboard and mouse inputs */
        processInput();
    }

    auto endDraw() const -> void {
        /* Swap front and back buffers */
        glfwSwapBuffers(_window.get());

        /* Poll for and process events */
        glfwPollEvents();
    }

private:
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> _window;

    auto processInput() const -> void {
        if (glfwGetKey(_window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(_window.get(), true);
        }
    }
};
