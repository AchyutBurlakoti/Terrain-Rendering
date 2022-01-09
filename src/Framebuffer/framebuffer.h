#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <iostream>
#include <string>

#define GLEW _STATIC
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

#include "../Others/Shader.h"

class frame_buffer
{
    unsigned int _frame_buffer;
    unsigned int _render_buffer_object;
    unsigned int _texture_buffer;
    unsigned int _quad_vao;
    unsigned int _quad_vbo;

    int _screen_width;
    int _screen_height;

    Shader* _frame_buffer_shader;

    bool _shader_status = false;

    float _quad[24] = {
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    public:

    frame_buffer() = default;

    frame_buffer(const std::string& vertex_shader_name, const std::string& fragment_shader_name)
    {
        bool static_draw = true;
        if(true)
        {
            _frame_buffer_shader = new Shader(vertex_shader_name, fragment_shader_name);
            _shader_status = true;
        }

        glGenVertexArrays(1, &_quad_vao);
        glGenBuffers(1, &_quad_vbo);

        glBindVertexArray(_quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _quad_vbo);

        if(static_draw) glBufferData(GL_ARRAY_BUFFER, sizeof(_quad), &_quad, GL_STATIC_DRAW);
        else            glBufferData(GL_ARRAY_BUFFER, sizeof(_quad), &_quad, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }


    void init(int width, int height)
    {
        _screen_width  = width;
        _screen_height = height;

        glGenFramebuffers(1, &_frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);  

        _texture_buffer = generate_texture(false, false);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture_buffer, 0);

        glGenRenderbuffers(1, &_render_buffer_object);
        glBindRenderbuffer(GL_RENDERBUFFER, _render_buffer_object); 

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _screen_width, _screen_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_buffer_object);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int get_texture()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

        // Clear all attached buffers

        if(_shader_status) _frame_buffer_shader->Use();

        glBindVertexArray(_quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return _texture_buffer;
    }

    void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
    }

    void unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    private:
    
    unsigned int generate_texture(bool depth, bool stencil)
    {
        GLenum attachment_type;
        unsigned int texture_id;

        if(!depth && !stencil) attachment_type = GL_RGB;
        else if( depth && !stencil) attachment_type = GL_DEPTH_COMPONENT;
        else if(!depth &&  stencil) attachment_type = GL_STENCIL_INDEX;

        //Generate texture ID and load texture data 
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        if(!depth && !stencil) glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, _screen_width, _screen_height, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
        else                   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _screen_width, _screen_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture  (GL_TEXTURE_2D, 0);

        return texture_id;
    }
};

#endif