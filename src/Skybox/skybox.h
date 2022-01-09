#ifndef SKYBOX_H
#define SKYBOX_H

#include <iostream>
#include <vector>
#include <memory>

#define GLEW _STATIC
#include <GL/glew.h>

#include "../Others/Shader.h"
#include "../Entity/entity.h"
#include "../Others/math.h"

class skybox : public entity
{
    int _screen_width;
    int _screen_height;

    std::unique_ptr<Shader> _skybox_shader;

    float _skybox_vertices1[108] = {
        
            // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float _skybox_vertices[108];


    unsigned int _skybox_vao;
    unsigned int _skybox_vbo;

    std::vector<const char*> _faces_textures;

    unsigned int _cubemap_texture;

    public:

    skybox() = default;

    skybox(int width, int height) : _screen_width(width), _screen_height(height)
    {

    }

    virtual void init() final
    {
        for(int i = 0; i < 108; i++)
        {
            _skybox_vertices[i] = 10000.0f * _skybox_vertices1[i];
        }
        _skybox_shader = std::unique_ptr<Shader>(new Shader("./src/Skybox/skybox.vs", "./src/Skybox/skybox.frag"));

        glGenVertexArrays(1, &_skybox_vao);
        glGenBuffers(1, &_skybox_vbo);

        glBindVertexArray(_skybox_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _skybox_vbo);

        if(true) glBufferData(GL_ARRAY_BUFFER, sizeof(_skybox_vertices), &_skybox_vertices, GL_STATIC_DRAW);
        else     glBufferData(GL_ARRAY_BUFFER, sizeof(_skybox_vertices), &_skybox_vertices, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    void set_skybox_texture(const char* right, const char* left, const char* top,
                             const char* bottom, const char* back, const char* front)
    {
        _faces_textures.push_back(right);
        _faces_textures.push_back(left);
        _faces_textures.push_back(top);
        _faces_textures.push_back(bottom);
        _faces_textures.push_back(back);
        _faces_textures.push_back(front);

        stbi_set_flip_vertically_on_load(1);

        _cubemap_texture = load_cubemap(_faces_textures);
    }

    virtual void render(glm::mat4 view, glm::vec3 dump_vec = glm::vec3(0.0f, 0.0f, 0.0f)) final
    {
        _skybox_shader->Use();

        view = glm::mat4(glm::mat3(view));
        glm::mat4 projection = perspective(glm::radians(30.0f), (float)_screen_width / (float)_screen_height, 0.1f, 20000.0f);

        glUniformMatrix4fv(glGetUniformLocation(_skybox_shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(_skybox_shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // skybox cube
        glBindVertexArray(_skybox_vao);
        glActiveTexture(GL_TEXTURE0);

        glUniform1i(glGetUniformLocation(_skybox_shader->Program, "skybox"), 0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemap_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
    }

    private:

    unsigned int load_cubemap(std::vector<const char*> faces)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);

        int width, height, m_BPP;

        unsigned char* image;
    
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        for(GLuint i = 0; i < faces.size(); i++)
        {
            image = stbi_load(faces[i], &width, &height, &m_BPP, 4);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture  (GL_TEXTURE_CUBE_MAP, 0);

        return texture_id;
    }
};

#endif