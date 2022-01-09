#version 330 core
layout (location = 0) in vec2 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightmap;

out vec2 uv;
out vec3 FragPos;
out vec3 Normal;

vec3 find_normal(sampler2D tex, float u)
{
    // u is one uint size, i.e. 1.0 / texture size
    vec2 offsets[4];
    offsets[0] = uv + vec2(-u, 0);
    offsets[1] = uv + vec2(u, 0);
    offsets[2] = uv + vec2(0, -u);
    offsets[3] = uv + vec2(0, u);

    float hts[4];
    for(int i = 0; i < 4; i++)
    {
        vec4 h_ = texture(tex, offsets[i]);
        hts[i] = mix(-10.0, 1000.0, h_.r);
    }
    
    vec2 _step = vec2(1.0, 0.0);
               
    vec3 va = normalize( vec3(_step.xy, hts[1] - hts[0]));
    vec3 vb = normalize( vec3(_step.yx, hts[3]-hts[2]));
               
    return cross(va,vb).rbg;
}

void main()
{
    Normal = find_normal(heightmap, 1.0);

    uv = position.xy / 2000.0;
	vec4 h = texture(heightmap, uv);
    float height = mix(-10.0, 1000.0, h.r);

    gl_Position =   projection * view * model *  vec4(position.x, height, position.y, 1.0f);

    FragPos = vec3(model * vec4(position.x, height, position.y, 1.0f));
}