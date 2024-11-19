module;
#include <GLFW/glfw3.h>

export module glfw;

auto _GLFW_CONTEXT_VERSION_MAJOR = GLFW_CONTEXT_VERSION_MAJOR;
#undef GLFW_CONTEXT_VERSION_MAJOR
export auto GLFW_CONTEXT_VERSION_MAJOR = _GLFW_CONTEXT_VERSION_MAJOR;

auto _GLFW_CONTEXT_VERSION_MINOR = GLFW_CONTEXT_VERSION_MINOR;
#undef GLFW_CONTEXT_VERSION_MINOR
export auto GLFW_CONTEXT_VERSION_MINOR = _GLFW_CONTEXT_VERSION_MINOR;

export using ::GLFWwindow;
export using ::glfwInit;
export using ::glfwMakeContextCurrent;
export using ::glfwTerminate;
export using ::glfwCreateWindow;
export using ::glfwWindowShouldClose;
export using ::glfwSwapBuffers;
export using ::glfwPollEvents;
export using ::glfwGetProcAddress;
export using ::glfwWindowHint;
export using ::glfwDestroyWindow;
