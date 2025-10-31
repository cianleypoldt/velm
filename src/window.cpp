#include "velm/window.h"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include "GLFW/glfw3native.h"
#include <cassert>

velm::window::window(int width, int height, char* title){
    glfwInit();
    handle = glfwCreateWindow(width, height,title, NULL, NULL);
    if(handle=nullptr) assert(0);
};

velm::window::~window(){
    glfwDestroyWindow((GLFWwindow*)handle);
}
