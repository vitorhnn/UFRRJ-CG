#version 330 core

in vec3 normal;

out vec4 color;

void main()
{
    vec3 lightdir = vec3(0.5, -5.3, 4.5); // random light direction values
    lightdir = normalize(-lightdir);

    float diff = max(dot(normal, lightdir), 0.0);

    color = diff * vec4(1.0, 1.0, 1.0, 0.0);
}
