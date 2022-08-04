#version 330 core

in vec2 tex_coord;
out vec4 color;

uniform vec4 mult_color;
uniform sampler2D texture_;

void main(){
    color = mult_color;

    //if texture is valid
    if (textureSize(texture_, 0) != vec2(1,1)){
        color *= texture(texture_, tex_coord);
    }
}