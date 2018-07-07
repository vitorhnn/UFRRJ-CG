#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 uv;

out vec4 color;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 ambient()
{
    float ambientMod = 0.1;
    vec3 ambient = ambientMod * lightColor * texture(diffuseMap, uv).rgb;

    return ambient;
}

vec3 diffuse()
{
    vec3 lightDir = normalize(lightPos - fragPos);
    float diffStr = max(dot(normalize(normal), lightDir), 0.0);

    return diffStr * lightColor * texture(diffuseMap, uv).rgb;
}

vec3 specular()
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(normal));

    float specStr = pow(max(dot(viewDir, reflectDir), 0.0), 96);

    vec3 specular = specStr * lightColor * texture(specularMap, uv).rgb;

    return specular;
}

void main()
{
    vec3 result = ambient() + diffuse() + specular();

    color = vec4(result, 1.0);
}
