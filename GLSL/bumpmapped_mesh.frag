#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec2 uv;
    vec3 tLightPos;
    vec3 tViewPos;
    vec3 tFragPos;
} fs_in;

out vec4 color;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform vec3 lightColor;

vec3 ambient()
{
    float ambientMod = 0.1;
    vec3 ambient = ambientMod * lightColor * texture(diffuseMap, fs_in.uv).rgb;

    return ambient;
}

vec3 diffuse(vec3 normal)
{
    vec3 lightDir = normalize(fs_in.tLightPos - fs_in.tFragPos);
    float diffStr = max(dot(lightDir, normal), 0.0);

    return diffStr * lightColor * texture(diffuseMap, fs_in.uv).rgb;
}

vec3 specular(vec3 normal)
{
    vec3 lightDir = normalize(fs_in.tLightPos - fs_in.tFragPos);
    vec3 viewDir = normalize(fs_in.tViewPos - fs_in.tFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float specStr = pow(max(dot(viewDir, reflectDir), 0.0), 96);

    vec3 specular = specStr * lightColor * texture(specularMap, fs_in.uv).rgb;

    return specular;
}

void main()
{
    // sample from the normal map
    vec3 normal = texture(normalMap, fs_in.uv).rgb;

    // map to [-1, 1]
    normal = normalize(normal * 2.0 - 1.0);

    vec3 result = ambient() + diffuse(normal) + specular(normal);

    color = vec4(result, 1.0);
}
