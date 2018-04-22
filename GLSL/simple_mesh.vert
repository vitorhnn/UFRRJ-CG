#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec3 norm;

out vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 invModel;

void main()
{
    normal = invModel * norm;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
