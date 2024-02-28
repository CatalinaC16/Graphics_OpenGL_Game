#include "Window.h"

namespace gps {

    void Window::Create(int width, int height, const char* title) {
        if (!glfwInit()) {
            throw std::runtime_error("Could not start GLFW3!");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

        glfwWindowHint(GLFW_SAMPLES, 4);

        this->window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!this->window) {
            throw std::runtime_error("Could not create GLFW3 window!");
        }

        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);

#if not defined (__APPLE__)
        glewExperimental = GL_TRUE;
        glewInit();
#endif
        const GLubyte* renderer = glGetString(GL_RENDERER); 
        const GLubyte* version = glGetString(GL_VERSION); 
        std::cout << "Renderer: " << renderer << std::endl;
        std::cout << "OpenGL version: " << version << std::endl;

        glfwGetFramebufferSize(window, &this->dimensions.width, &this->dimensions.height);
    }

    void Window::Delete() {
        if (window)
            glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWwindow* Window::getWindow() {
        return this->window;
    }

    WindowDimensions Window::getWindowDimensions() {
        return this->dimensions;
    }

    void Window::setWindowDimensions(WindowDimensions dimensions) {
        this->dimensions = dimensions;
    }
}
