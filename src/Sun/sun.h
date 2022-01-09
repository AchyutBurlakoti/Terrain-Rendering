#ifndef SUN_H
#define SUN_H

#include <iostream>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include "../Others/Shader.h"
#include "../Entity/entity.h"
#include "../Others/math.h"

class sun : public entity
{
    unsigned int vao;
    unsigned int vbo;

    int _screen_height;
    int _screen_width;

    std::unique_ptr<Shader> _sun_shader;

    std::vector<float> sun_vertices;
    std::vector<float> vertices;
    std::vector<float> temp;

    int n;

    glm::vec3 sun_position;

    public:

    sun(int width, int height) : _screen_width(width), _screen_height(height)
    {
        _sun_shader = std::unique_ptr<Shader>(new Shader("./src/Sun/light.vs", "./src/Sun/light.frag"));

        sun_vertices = sphere();
    }

    virtual void init() final
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sun_vertices.size(), &sun_vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glBindVertexArray(0);
    }

    virtual void render(glm::mat4 view, glm::vec3 light_pos) final
    {
        sun_position = light_pos;

        glm::mat4 model(1.0f);
        model = translate(model, sun_position);

        glm::mat4 projection = perspective(glm::radians(30.0f), (float)_screen_width / (float)_screen_height, 0.1f, 10000.0f);

        _sun_shader->Use();
        glBindVertexArray(vao);

        glUniformMatrix4fv(glGetUniformLocation(_sun_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(_sun_shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(_sun_shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, sun_vertices.size());
        glBindVertexArray(0);
    }

    private :

    void sphere(float phi, float theta, float radius = 100.0f)
    {
        theta = theta * (3.14 / 180);
        phi = phi * (3.14 / 180);

        float z = radius * cos(theta);
        float y = radius * sin(phi) * sin(theta);
        float x = radius * cos(phi) * sin(theta);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }

    void indexing()
    {
        int count = 0; 
        int check = 0;
        for(int i = 0; i < vertices.size(); i++)
        {
            if (count == 9)
            {
                count = 0;
                if(check % 2 == 0)
                {
                    i = i - 6;
                    check++;
                }
                else
                {
                    check++;
                }
            }
            temp.push_back(vertices[i]);
            count++;
        }
    }

    void reduce(int n, int m)
    {
        for(int i = n; i <= n + 10; i += 10)
        {
            for(int j = m; j <= m + 10; j += 10)
            {
                sphere(i, j);
            }
        }
    }

    std::vector<float> sphere()
    {
        for(int i = 0; i < 360; i += 10)
        {
            for(int j = 0; j < 360; j += 10)
            {
                reduce(i, j);
            }
        }
        indexing();
        return temp;
    }
};

#endif