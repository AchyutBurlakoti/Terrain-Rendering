#ifndef CAMERA_HPP
#define CAMERA_HPP
// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "math.h"

enum camera_movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class camera
{
    glm::vec3 camera_front      = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_pos_change = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up         = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 camera_right;
    glm::vec3 camera_direction;
    glm::vec3 light_pos;

    float heading_angle = 0.0f;
    float pitch_angle   = 0.0f;
    float max_pitch     = 5;
    float max_head      = 5;
    float speed         = 1.0f;

    glm::quat heading_quat;
    glm::quat pitch_quat;

    public:

    glm::vec3 camera_pos        = glm::vec3(0.0f, 0.0f, 3.0f);

    camera(glm::vec3 lightpos = glm::vec3(1.2f, 1.0f, 2.0f)) : light_pos(lightpos)
    {
        camera_direction = glm::normalize(camera_front - camera_pos);
    }

    void use()
    {
        camera_right     = glm::cross(camera_direction, camera_up);

        heading_quat     = glm::angleAxis(glm::radians(heading_angle), camera_up);
        pitch_quat       = glm::angleAxis(glm::radians(pitch_angle), glm::normalize(camera_right));

        camera_pos       = camera_pos + camera_pos_change;
        camera_front     = camera_direction * 1.0f + camera_pos;

        // for smoothness
        camera_pos_change *= .01;
    }

    glm::mat4 camera_view()
    {
        return lookAt(camera_pos, camera_front, camera_up);
    }

    void process_keyboard(camera_movement direction, float delta_time)
    {
        GLfloat velocity = speed * delta_time;
        if (direction == FORWARD)
        {
            camera_pos_change += camera_direction;
            camera_pos_change *= velocity;
        }
        if (direction == BACKWARD)
        {
            camera_pos_change -= camera_direction;
            camera_pos_change *= velocity;
        }
        if (direction == LEFT)
        {
            camera_pos_change -= glm::cross(camera_direction, camera_up);
            camera_pos_change *= velocity;
        }
        if (direction == RIGHT)
        {
            camera_pos_change += glm::cross(camera_direction, camera_up);
            camera_pos_change *= velocity;        
        }
    }

    void process_mouse_movement(float xoffset, float yoffset)
    {
        xoffset = 0.3 * xoffset;
        yoffset = 0.3 * yoffset;

        camera_direction = glm::rotate(heading_quat, camera_direction);
        camera_direction = glm::rotate(pitch_quat, camera_direction);
        
        camera_direction = glm::normalize(camera_direction);
 
        heading_angle  = 0;
        heading_angle -= xoffset;

        pitch_angle  = 0;
        pitch_angle += yoffset;
    }
};

#endif