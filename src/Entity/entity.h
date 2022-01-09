#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class entity
{
    public :

    entity() = default;

    virtual void init() = 0;

    virtual void render(glm::mat4 view, glm::vec3 light_pos) = 0;
};

#endif