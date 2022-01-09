#version 330 core
out vec4 color;

in vec2 uv;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D snow;
uniform sampler2D depthMap;
uniform sampler2D white_snow;
uniform sampler2D blendMap;

uniform vec3 lightPos;

void main()
{    
    vec3 light_color = vec3(1.0, 1.0, 1.0);

    float new_depth_color = pow(gl_FragCoord.z, 5000);
    vec2 a = vec2(gl_FragCoord.x / 1366.0, gl_FragCoord.y / 768.0);
    color = texture(depthMap, a);
    float prev_depth_color = color.x;

    if(new_depth_color <= prev_depth_color + 0.0095f)
    {

        // Texture mapping part......
        vec4 blendmapcolor = texture(blendMap, uv);
        float black_texture_amount = 1 - (blendmapcolor.r + blendmapcolor.g + blendmapcolor.b);

        vec2 tiledCoords = uv * 3.0;
        vec4 black_texture_color = texture(snow, tiledCoords) * black_texture_amount;
        vec4 red_texture_color = texture(white_snow, tiledCoords) *  blendmapcolor.r;
        vec4 green_texture_color = texture(white_snow, tiledCoords) * blendmapcolor.g;
        vec4 blue_texture_color = texture(snow, tiledCoords) * blendmapcolor.b;

        vec4 total_colour = black_texture_color + red_texture_color + green_texture_color + blue_texture_color;


        // Ambient
        vec3 strength = vec3(0.2f);
        vec3 ambient = strength * light_color;

        // Diffuse 
        vec3 diffusive = vec3(0.8);
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * diffusive * light_color;

        // Specular
        float specularStrength = 32.0;
        vec3 viewPos = vec3(0.0, 0.0, 0.0);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * light_color;

        color = vec4(specular, 1.0f) * total_colour;
    }
    else
    {
        color = texture(depthMap, a);
    }
}