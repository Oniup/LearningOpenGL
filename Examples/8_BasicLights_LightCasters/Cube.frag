#version 450 core

#define MAX_LIGHT_COUNT_DIRECTIONAL 4
#define MAX_LIGHT_COUNT_POINT 10
#define MAX_LIGHT_COUNT_SPOT 10

struct DirectionalLight
{
    vec3 Direction;
    vec3 Color;
    vec3 AmbientColor;
    float Intensity;
};

struct PointLight
{
    vec3 Position;
    vec3 Color;
    float Intensity;
    float Linear;
    float Quadratic;
};

struct SpotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float Intensity;
    float CutOff;
    float Linear;
    float Quadratic;
};

struct Material
{
    sampler2D DiffuseMap;
    sampler2D SpecularMap;
    float Shininess;

    bool EnableEmissionMap;
    sampler2D EmissionMap;
};

layout (std140, binding = 1) uniform Lights
{
    DirectionalLight Directionals[MAX_LIGHT_COUNT_DIRECTIONAL];
    PointLight Points[MAX_LIGHT_COUNT_POINT];
    SpotLight Spots[MAX_LIGHT_COUNT_SPOT];
    uint DirectionalCount;
    uint PointCount;
    uint SpotCount;
};

// uniform vec3 u_AmbientLight;
uniform vec3 u_ViewPosition;
uniform Material u_Material;

in VS_VERTEX
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} Vertex;

out vec4 FragColor;

vec3 DirectionalLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection);
vec3 PointLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection);
vec3 SpotLights(vec3 diffuseMap, vec3 specularMap);

void main()
{
    vec4 diffuseMap = texture(u_Material.DiffuseMap, Vertex.UV);
    vec4 specularMap = texture(u_Material.SpecularMap, Vertex.UV);
    vec3 viewDirection = normalize(u_ViewPosition - Vertex.Position);

    vec3 pointLighting = PointLights(diffuseMap.rgb, specularMap.rgb, viewDirection);
    vec3 directionalLighting = DirectionalLights(diffuseMap.rgb, specularMap.rgb, viewDirection);
    FragColor = vec4(pointLighting + directionalLighting, 1.0);

    if (u_Material.EnableEmissionMap)
    {
        vec4 emissionMap = texture(u_Material.EmissionMap, Vertex.UV);
        FragColor += emissionMap;
    }
}

vec3 CalcDiffuse(vec3 lightDirection, vec3 lightColor)
{
    float diffuse = clamp(dot(Vertex.Normal, lightDirection), 0.0, 1.0);
    return diffuse * lightColor;
}

vec3 CalcSpecular(vec3 lightDirection, vec3 lightColor, vec3 viewDirection)
{
    vec3 reflectDirection = reflect(-lightDirection, Vertex.Normal);
    float specular = pow(clamp(dot(viewDirection, reflectDirection), 0.0, 1.0), u_Material.Shininess);
    return specular * lightColor;
}

float CalcAttenuation(vec3 lightPosition, float linear, float quadratic)
{
    float dist = length(lightPosition - Vertex.Position);
    return 1.0 / (1.0 + linear * dist + quadratic * (dist * dist));
}

vec3 DirectionalLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection)
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    for (int i = 0; i < DirectionalCount; ++i)
    {
        vec3 lightDirection = -Directionals[i].Direction;
        vec3 diffuseLighting = CalcDiffuse(lightDirection, Directionals[i].Color);
        vec3 specularLighting = CalcSpecular(lightDirection, Directionals[i].Color, viewDirection);

        ambient += Directionals[i].AmbientColor;
        diffuse += diffuseLighting * Directionals[i].Intensity;
        specular += specularLighting * Directionals[i].Intensity;
    }
    ambient *= diffuseMap;
    diffuse *= diffuseMap;
    specular *= specularMap * 0.2;
    return ambient + diffuse + specular;
}

vec3 PointLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection)
{
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    for (uint i = 0; i < PointCount; ++i)
    {
        vec3 lightDirection = normalize(Points[i].Position - Vertex.Position);
        vec3 diffuseLighting = CalcDiffuse(lightDirection, Points[i].Color);
        vec3 specularLighting = CalcSpecular(lightDirection, Points[i].Color, viewDirection);
        float attenuation = CalcAttenuation(Points[i].Position, Points[i].Linear, Points[i].Quadratic);

        diffuse += diffuseLighting * attenuation * Points[i].Intensity;
        specular +=  specularLighting * attenuation * Points[i].Intensity;
    }
    diffuse *= diffuseMap;
    specular *= specularMap;
    return diffuse + specular;
}

vec3 SpotLights(vec3 diffuseMap, vec3 specularMap)
{
    return vec3(0.0);
}
