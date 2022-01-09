#version 330 core

out vec4 color;
in vec2 uv;

uniform sampler2D depthMap;

void main()
{    
    float depthValue = pow(gl_FragCoord.z, 5000);
    color = vec4(vec3(depthValue), 1.0f);
}
