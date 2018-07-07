#version 330 core

// vertex shader: forward rendered phong with diffuse, specular and normal mapping, single light source

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out VS_OUT {
    vec3 fragPos;
    vec2 uv;
    vec3 tLightPos;
    vec3 tViewPos;
    vec3 tFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 invModel;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.fragPos = vec3(model * vec4(pos, 1.0));
    vs_out.uv = uv;

    // calculate tangent space matrix
    vec3 T = normalize(invModel * tangent);
    vec3 N = normalize(invModel * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    // i forgot to type transpose here and wasted 2 hours of my life
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.tLightPos = TBN * lightPos;
    vs_out.tViewPos = TBN * viewPos;
    vs_out.tFragPos = TBN * vs_out.fragPos;


    gl_Position = projection * view * model * vec4(pos, 1.0);
}
