module;
#include <GLFW/glfw3.h>

export module glfw;

auto _GLFW_CONTEXT_VERSION_MAJOR = GLFW_CONTEXT_VERSION_MAJOR;
#undef GLFW_CONTEXT_VERSION_MAJOR
export auto GLFW_CONTEXT_VERSION_MAJOR = _GLFW_CONTEXT_VERSION_MAJOR;

auto _GLFW_CONTEXT_VERSION_MINOR = GLFW_CONTEXT_VERSION_MINOR;
#undef GLFW_CONTEXT_VERSION_MINOR
export auto GLFW_CONTEXT_VERSION_MINOR = _GLFW_CONTEXT_VERSION_MINOR;

export constexpr auto _GLFW_KEY_ESCAPE = GLFW_KEY_ESCAPE;
#undef GLFW_KEY_ESCAPE
export constexpr auto GLFW_KEY_ESCAPE = _GLFW_KEY_ESCAPE;

export constexpr auto _GLFW_PRESS = GLFW_PRESS;
#undef GLFW_PRESS
export constexpr auto GLFW_PRESS = _GLFW_PRESS;

export using ::GLFWwindow;
export using ::glfwInit;
export using ::glfwMakeContextCurrent;
export using ::glfwTerminate;
export using ::glfwCreateWindow;
export using ::glfwWindowShouldClose;
export using ::glfwGetKey;
export using ::glfwSetWindowShouldClose;
export using ::glfwSetFramebufferSizeCallback;
export using ::glfwSetKeyCallback;
export using ::glfwSetCursorPosCallback;
export using ::glfwSetMouseButtonCallback;
export using ::glfwSetScrollCallback;
export using ::glfwSetWindowUserPointer;
export using ::glfwGetWindowUserPointer;
export using ::glfwSwapBuffers;
export using ::glfwPollEvents;
export using ::glfwGetProcAddress;
export using ::glfwWindowHint;
export using ::glfwDestroyWindow;
