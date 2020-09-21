#include <world/world.h>

#include <GLFW/glfw3.h>

int main() {
    glfwInit();
    World().start();
    glfwTerminate();
    return 0;
}
