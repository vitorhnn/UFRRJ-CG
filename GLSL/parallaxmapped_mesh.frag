#version 330 core

// fragment shader: forward rendered phong with diffuse, specular, normal mapping and parallax mapping, single light source

in VS_OUT {
    vec3 fragPos;
    vec2 uv;
    vec3 tLightPos;
    vec3 tViewPos;
    vec3 tFragPos;
} fs_in;

out vec4 color;

uniform vec3 lightColor;

uniform float heightScale;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;


vec3 ambient(vec2 uv)
{
    float ambientMod = 0.1;
    vec3 ambient = ambientMod * lightColor * texture(diffuseMap, uv).rgb;

    return ambient;
}

vec3 diffuse(vec2 uv, vec3 normal)
{
    vec3 lightDir = normalize(fs_in.tLightPos - fs_in.tFragPos);
    float diffStr = max(dot(lightDir, normal), 0.0);

    return diffStr * lightColor * texture(diffuseMap, uv).rgb;
}

vec3 specular(vec2 uv, vec3 normal)
{
    vec3 lightDir = normalize(fs_in.tLightPos - fs_in.tFragPos);
    vec3 viewDir = normalize(fs_in.tViewPos - fs_in.tFragPos);
    vec3 halfway = normalize(lightDir + viewDir);

    float specStr = pow(max(dot(normal, halfway), 0.0), 96);

    vec3 specular = specStr * lightColor * texture(specularMap, uv).rgb;

    return specular;
}

vec2 parallax()
{
    vec3 viewDir = normalize(fs_in.tViewPos - fs_in.tFragPos);
    float height = texture(depthMap, fs_in.uv).r;
    vec2 p = viewDir.xy * (height * heightScale);

    return fs_in.uv - p;
}

void main()
{
    vec2 modifiedUv = parallax();
//    vec2 modifiedUv = fs_in.uv;

    if (modifiedUv.x > 1.0 || modifiedUv.y > 1.0 || modifiedUv.x < 0.0 || modifiedUv.y < 0.0) {
        discard;
    }

    // sample from the normal map
    vec3 normal = texture(normalMap, modifiedUv).rgb;

    // map to [-1, 1]
    normal = normalize(normal * 2.0 - 1.0);

    vec3 result = ambient(modifiedUv) + diffuse(modifiedUv, normal) + specular(modifiedUv, normal);

    color = vec4(result, 1.0);
}
