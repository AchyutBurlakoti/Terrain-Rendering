#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

/* Singleton structure */
struct window
{
    GLFWwindow* glfw_window;

    uint16_t height;
    uint16_t width;

    char *name_of_window;

    bool anti_aliasing;
    bool resizable;
    bool cursor_status;
};

window* get_window()
{
    static window *s_window;

    if (s_window == nullptr)
    {
        s_window = (window*)malloc(sizeof(window));
    }

    return s_window;
}

window* init_window(uint16_t width, uint16_t height, char* name_of_window, bool anti_aliasing = false, bool resizable = true, bool cursor_status = true)
{
    window *window;

    window = get_window();

    window->height         = height;
    window->width          = width;
    window->name_of_window = name_of_window;
    window->anti_aliasing  = anti_aliasing;
    window->resizable      = resizable;
    window->cursor_status  = cursor_status;

    return window;
}

void create_window()
{
    window* w = get_window();

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, w->resizable);
    glfwWindowHint(GLFW_SAMPLES, 4);

    w->glfw_window = glfwCreateWindow(w->width, w->height, w->name_of_window, nullptr, nullptr);

    glfwMakeContextCurrent(w->glfw_window);

    glfwSetInputMode(w->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) std::cout << "Failed to initialize GLEW" << std::endl;

    glViewport(0, 0, w->width, w->height);
}

#endif