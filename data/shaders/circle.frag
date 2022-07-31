#version 330 core

in vec2 tex_coord;
out vec4 color;

uniform sampler2D texture_;
uniform vec4 mult_color;
uniform vec3 scale;
uniform bool keep_aspect;
uniform float smoothness = 0.1;
uniform float thickness = 1.0;

void main()
{
    vec2 uv = (tex_coord - 0.5) * 2;
    float aspect = scale.x / scale.y;

    if (keep_aspect){
        uv.x *= aspect;
    }

    float distance = 1.0 - length(uv);
    color = vec4(smoothstep(0.0, smoothness, distance));
    color *= vec4(smoothstep(thickness + smoothness , thickness, distance));

    // verify if texture is valid
    if (textureSize(texture_, 0) != vec2(1,1)){
        color *= texture(texture_, tex_coord);
    }
    
    color *= mult_color;
}