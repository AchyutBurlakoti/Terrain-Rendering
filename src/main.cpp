#include "Window/window.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Terrain/terrain.hpp"
#include "Others/camera.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();
void render_quad();

// Camera
camera camera;

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLuint screenWidth = 1366, screenHeight = 768;

GLfloat deltaTime = 5.0f;
GLfloat lastFrame = 0.0f;

int main()
{
    window* w;
    w = get_window();
    init_window(screenWidth, screenHeight, (char*)"Terrian", true, true, true);
    create_window();

    glfwSetKeyCallback(w->glfw_window, key_callback);
    glfwSetCursorPosCallback(w->glfw_window, mouse_callback);

    terrain terrain(screenWidth, screenHeight, 2000, 300);
    terrain.init();

    glm::vec3 position(-100.0f, 500.0f, -500.0f);

    float radius = 1700.0f;
    while(!glfwWindowShouldClose(w->glfw_window))
    {
        glfwPollEvents();
        do_movement();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float timer = glfwGetTime();
        float angle = glm::radians(timer) * 15;
        position = glm::vec3(position.x, radius * glm::sin(angle), radius * glm::cos(angle));

        camera.use();
        glm::mat4 view = camera.camera_view();

        terrain.render(view, position);
        
        glfwSwapBuffers(w->glfw_window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

void do_movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
    {
        camera.process_keyboard(FORWARD, deltaTime);
    }
    if(keys[GLFW_KEY_S])
    {
        camera.process_keyboard(BACKWARD, deltaTime);
    }
    if(keys[GLFW_KEY_A])
    {
        camera.process_keyboard(LEFT, deltaTime);
    }
    if(keys[GLFW_KEY_D])
    {
        camera.process_keyboard(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;

    camera.process_mouse_movement(xoffset, yoffset);
}