#version 330 core
out vec4 color;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{    
    float new_depth_color = pow(gl_FragCoord.z, 5000);
    vec2 a = vec2(gl_FragCoord.x / 1366.0, gl_FragCoord.y / 768.0);
    color = texture(depthMap, a);
    float prev_depth_color = color.x;

    if(new_depth_color <= prev_depth_color + 0.0095)
    {
        color = vec4(vec3(new_depth_color), 1.0f);
    }
    else
    {
        color = vec4(vec3(prev_depth_color), 1.0f);
    }
}