#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 in_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex_coord;

void main(){
    tex_coord = in_tex_coord;
    gl_Position = projection * view * model * vec4(vertex_position, 1);
}