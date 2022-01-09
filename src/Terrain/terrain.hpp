#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>
#include <vector>
#include <memory>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include "../Entity/entity.h"

#include "../Framebuffer/framebuffer.h"

#include "../Skybox/skybox.h"
#include "../Sun/sun.h"

#include "../Others/Shader.h"
#include "../Others/math.h"

struct terrain_vertices
{
    float x, z;
};

class terrain : public entity
{
    int _screen_height;
    int _screen_width;

    int size;
    int actual_size;

    int _length;
    int N;

    std::unique_ptr<frame_buffer> _height_map;
    std::unique_ptr<frame_buffer> _blend_map;

    std::unique_ptr<terrain_vertices[]> vertices;
    std::unique_ptr<terrain_vertices[]> final_vertices;

    unsigned int vao;
    unsigned int vbo;

    unsigned int white_snow_texture;
    unsigned int snow_texture;

    std::unique_ptr<Shader> depth_shader_one;
    std::unique_ptr<Shader> depth_shader_two;
    std::unique_ptr<Shader> texture_mapping_one;
    std::unique_ptr<Shader> texture_mapping_two;
    std::unique_ptr<Shader> quad_shader;

    std::unique_ptr<skybox> skybox_;

    std::unique_ptr<sun> sun_;

    GLfloat quadVertices[24] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    GLuint quadVAO, quadVBO;
    GLuint framebuffer, framebuffer_, rbo, rbo_, depthMap, depthMap_;

    GLuint height_map_texture;
    GLuint blend_map_texture;

    public:

    terrain(int width, int height, int length, int number_of_squares)
    {
        skybox_ = std::unique_ptr<skybox>(new skybox(width, height));
        sun_ = std::unique_ptr<sun>(new sun(width, height));    

        white_snow_texture      = get_texture_id("./assets/textures/white_snow.jpg");
        snow_texture            = get_texture_id("./assets/textures/snow.jpg");

        _height_map = std::unique_ptr<frame_buffer>(new frame_buffer("./src/Terrain/Shaders/noise.vs", "./src/Terrain/Shaders/noise.frag"));
        _blend_map  = std::unique_ptr<frame_buffer>(new frame_buffer("./src/Terrain/Shaders/noise.vs", "./src/Terrain/Shaders/blend.frag"));

        depth_shader_one    = std::unique_ptr<Shader>(new Shader("./src/Terrain/Shaders/advance.vs"    , "./src/Terrain/Shaders/advance.frag"));
        depth_shader_two    = std::unique_ptr<Shader>(new Shader("./src/Terrain/Shaders/depth_one.vs"  , "./src/Terrain/Shaders/depth_one.frag"));
        quad_shader         = std::unique_ptr<Shader>(new Shader("./src/Terrain/Shaders/screen.vs"     , "./src/Terrain/Shaders/screen.frag"));
        texture_mapping_one = std::unique_ptr<Shader>(new Shader("./src/Terrain/Shaders/texture_one.vs", "./src/Terrain/Shaders/texture_one.frag"));
        texture_mapping_two = std::unique_ptr<Shader>(new Shader("./src/Terrain/Shaders/texture_two.vs", "./src/Terrain/Shaders/texture_two.frag"));

        _screen_width  = width;
        _screen_height = height;

        size     = (number_of_squares + 1) * (number_of_squares + 1);
        vertices = std::unique_ptr<terrain_vertices[]>(new terrain_vertices[size]);

        actual_size    = 6 * number_of_squares * number_of_squares;
        final_vertices = std::unique_ptr<terrain_vertices[]>(new terrain_vertices[actual_size]);

        _length = length;
        N       = number_of_squares;

        int index = 0;

        for(float m = N / 2.0f; m < N / 2 + N + 1; m += 1.0f)
        {
            for(float n = N / 2.0f; n < N / 2 + N + 1; n += 1.0f)
            {
                vertices[index].x = (n - N / 2.0f) * length / N;
                vertices[index].z = (m - N / 2.0f) * length / N;
                index++;
            }
        }
        set_indices();
    }

    virtual void init() final
    {
        _height_map->init(_screen_width, _screen_height);
        _blend_map->init(_screen_width, _screen_height);

        height_map_texture = _height_map->get_texture();
        blend_map_texture = _blend_map->get_texture();

        skybox_->init();
        skybox_->set_skybox_texture("./assets/textures/stormydays_rt.jpg",
                                    "./assets/textures/stormydays_lf.jpg",
                                    "./assets/textures/stormydays_dn.jpg",
                                    "./assets/textures/stormydays_up.jpg",
                                    "./assets/textures/stormydays_bk.jpg",
                                    "./assets/textures/stormydays_ft.jpg");  

        sun_->init();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(terrain_vertices) * actual_size, &final_vertices[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(terrain_vertices), (GLvoid*)0);

        glBindVertexArray(0);

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  

        // Create a color attachment texture
        depthMap = generateAttachmentTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap, 0);

        // Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _screen_width, _screen_height); // Use a single renderbuffer object for both a depth AND stencil buffer.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it

        // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &framebuffer_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);  

        // Create a color attachment texture
        depthMap_ = generateAttachmentTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap_, 0);

        // Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _screen_width, _screen_height); // Use a single renderbuffer object for both a depth AND stencil buffer.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_); // Now actually attach it
        
        // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual void render(glm::mat4 view, glm::vec3 light_pos) final
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 model(1.0f);
        model = translate(model, glm::vec3(-1000.0f, -500.0f, -1500.0f));
        model = rotate(model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 projection = perspective(glm::radians(30.0f), (float)_screen_width / (float)_screen_height, 0.1f, 2000.0f);

        depth_shader_one->Use();
        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, height_map_texture);
        glUniform1i(glGetUniformLocation(depth_shader_one->Program, "heightmap"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(depth_shader_one->Program, "depthMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(depth_shader_one->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(depth_shader_one->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(depth_shader_one->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, actual_size);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        depth_shader_two->Use();
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, height_map_texture);
        glUniform1i(glGetUniformLocation(depth_shader_two->Program, "heightmap"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(depth_shader_two->Program, "depthMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(depth_shader_two->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(depth_shader_two->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(depth_shader_two->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for(int i = 0; i < actual_size; i+=3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3);
        }

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        texture_mapping_one->Use();
        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, height_map_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_one->Program, "heightmap"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, snow_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_one->Program, "snow"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(texture_mapping_one->Program, "depthMap"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, blend_map_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "blendMap"), 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, white_snow_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "white_snow"), 4);

        glUniform3f(glGetUniformLocation(texture_mapping_one->Program, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

        glUniformMatrix4fv(glGetUniformLocation(texture_mapping_one->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(texture_mapping_one->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(texture_mapping_one->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for(int i = 0; i < actual_size; i+=3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3);
        }

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        skybox_->render(view);
        sun_->render(view, light_pos);

        texture_mapping_two->Use();
        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, height_map_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "heightmap"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, snow_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "snow"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "depthMap"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap_);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "depthMap_"), 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, blend_map_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "blendMap"), 4);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, white_snow_texture);
        glUniform1i(glGetUniformLocation(texture_mapping_two->Program, "white_snow"), 5);

        glUniform3f(glGetUniformLocation(texture_mapping_two->Program, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

        glUniformMatrix4fv(glGetUniformLocation(texture_mapping_two->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(texture_mapping_two->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(texture_mapping_two->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for(int i = 0; i < actual_size; i+=3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3);
        }

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        quad_shader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap_);
        glUniform1i(glGetUniformLocation(quad_shader->Program, "depthMap"), 0);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    private:

    void set_indices()
    {
        int index_count = 0;
        int index = 0;

        for(int m = 0; m < N; m++)
        {
        // sentinel to make sure that we will go to next row after indexing the last rectangle.
            if(m != 0) index_count++;

            for(int n = 0; n < N; n++)
            {
                {
                    int original_index = index_count;

                    final_vertices[index].x =  vertices[original_index].x;
                    final_vertices[index].z =  vertices[original_index].z;

                    index++;
                }

                {
                    int original_index = index_count + ( N + 1);
                
                    final_vertices[index].x =  vertices[original_index].x;
                    final_vertices[index].z =  vertices[original_index].z;

                    index++;
                }

                {
                    int original_index = index_count + ( N + 1) + 1;
                
                    final_vertices[index].x =  vertices[original_index].x;
                    final_vertices[index].z =  vertices[original_index].z;

                    index++;
                }

                {
                    int original_index = index_count;
                
                    final_vertices[index].x =  vertices[original_index].x;
                    final_vertices[index].z =  vertices[original_index].z;

                    index++;
                }

                {
                    int original_index = index_count + 1;
                
                    final_vertices[index].x =  vertices[original_index].x;
                    final_vertices[index].z =  vertices[original_index].z;

                    index++;
                }

                {
                    int original_index = index_count + ( N + 1) + 1;
                
                    final_vertices[index].x =  vertices[original_index].x;
                    final_vertices[index].z =  vertices[original_index].z;

                    index++;
                }
                index_count++;
            }
        }
    }

    unsigned int get_texture_id(const char* path)
    {
        unsigned int texture_id;

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id); 

        // Set our texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load, create texture and generate mipmaps
        int width, height, m_BPP;
        stbi_set_flip_vertically_on_load(1);
        unsigned char *image = stbi_load(path, &width, &height, &m_BPP, 4);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture_id;
    }

    GLuint generateAttachmentTexture()
    {
        //Generate texture ID and load texture data 
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _screen_width, _screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        return textureID;
    }
   
};

#endif