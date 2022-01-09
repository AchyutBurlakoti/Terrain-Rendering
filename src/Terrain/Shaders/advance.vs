#version 330 core
layout (location = 0) in vec2 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightmap;

out vec2 uv;

void main()
{
    uv = position.xy / 2000.0;
	vec4 h = texture(heightmap, uv);
    float height = mix(-10.0, 1000.0, h.r);

    gl_Position =   projection * view * model *  vec4(position.x, height, position.y, 1.0f);
}