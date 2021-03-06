#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
    color = texture(depthMap, TexCoords);
}

