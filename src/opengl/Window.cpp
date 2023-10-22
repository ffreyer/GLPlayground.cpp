#include "Window.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool Window::init()
{
    // Init GLFW
    if (!glfwInit()) {
        std::cout << "Failed to Init GLFW" << std::endl;
        return false;
    }
    
    // Init OpenGL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    m_window = glfwCreateWindow(m_window_size.x, m_window_size.y, m_name.c_str(), NULL, NULL);
    if (m_window == nullptr) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    
    activate();

    // vsync on by default
    glfwSwapInterval(1);

    // Init OpenGL interface
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // set initial viewport
    glViewport(0, 0, m_window_size.x, m_window_size.y);
    // update on resize
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    return true;
}

void Window::activate()
{
    glfwMakeContextCurrent(m_window);
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers(m_window);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(m_window);
}

glm::vec2 Window::get_mouse_position() {
    double mx, my;
    glfwGetCursorPos(m_window, &mx, &my);
    return glm::vec2(mx, my);
    // Or just this?
    // return m_mouse_position;
}
glm::ivec2 Window::get_window_position() {
    return m_window_position;
}
glm::ivec2 Window::get_window_size() {
    return m_window_size;
}

void Window::set_vsync(bool active)
{
    activate(); // for savety
    glfwSwapInterval(active ? 1 : 0);
}

void Window::set_size(int width, int height)
{
    activate(); // for savety
}


// TODO:
// create better event system



