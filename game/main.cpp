#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

int main() {
    if (!glfwInit()) {
        spdlog::error("Failed to init GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // bez OpenGL
    GLFWwindow* window = glfwCreateWindow(1280, 720, "drago", nullptr, nullptr);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}