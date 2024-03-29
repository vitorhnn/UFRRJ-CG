#version 330 core

// vertex shader: forward rendered phong with diffuse and specular mapping, single light source

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 iUV;
layout (location = 2) in vec3 norm;

out vec3 normal;
out vec3 fragPos;
out vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 invModel;

void main()
{
    normal = invModel * norm;
    fragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * model * vec4(pos, 1.0);

    uv = iUV;
}
