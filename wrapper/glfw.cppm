module;
#include <GLFW/glfw3.h>

export module glfw;

export using ::GLFWwindow;
export using ::glfwInit;
export using ::glfwMakeContextCurrent;
export using ::glfwTerminate;
export using ::glfwCreateWindow;
export using ::glfwWindowShouldClose;
export using ::glfwSwapBuffers;
export using ::glfwPollEvents;
export using ::glfwGetProcAddress;