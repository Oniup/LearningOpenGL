#version 450 core

#define MAX_TEXTURE_MAPS 4

#define MAX_DIRECTIONAL_LIGHT_COUNT 5
#define MAX_POINT_LIGHT_COUNT 20
#define MAX_SPOT_LIGHT_COUNT 20

#define AMBIENT_INTENSITY_PERCENTAGE 0.5

out vec4 FragColor;

struct DirectionalLight
{
    vec3 Direction;
    vec3 BaseColor;
    vec3 AmbientColor;
    vec3 SpecularColor;
    float Intensity;
};

struct PointLight
{
    vec3 Position;
    vec3 BaseColor;
    vec3 AmbientColor;
    vec3 SpecularColor;
    float Intensity;
    float Linear;
    float Quadratic;
};

struct SpotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 BaseColor;
    vec3 AmbientColor;
    vec3 SpecularColor;
    float Intensity;
    float Linear;
    float Quadratic;
    float CutOff;
    float OuterCutOff;
};

struct Lights
{
    PointLight PointLights[MAX_POINT_LIGHT_COUNT];
    DirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
    SpotLight SpotLights[MAX_SPOT_LIGHT_COUNT];
    uint PointCount;
    uint DirectionalCount;
    uint SpotCount;
};

struct Material
{
    uint DiffuseCount;
    uint SpecularCount;
    uint EmissionCount;
    sampler2D DiffuseMaps[MAX_TEXTURE_MAPS];
    sampler2D SpecularMaps[MAX_TEXTURE_MAPS];
    sampler2D EmissionMaps[MAX_TEXTURE_MAPS];
};

// From vertex shader
in VS_VERTEX
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} Vertex;

layout (std140, binding = 1) uniform ub_Lights
{
    Lights Light;
};

// Uniforms
uniform Material u_Material;
uniform vec3 u_ViewPosition;

// Utility Functions 
vec3 GetTexture(sampler2D textureSample);

// Lighting Functions
vec3 PointLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection);
vec3 DirectionalLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection);
vec3 SpotLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection);

vec3 DiffuseLighting(vec3 lightDirection, vec3 lightColor);
vec3 SpecularLighting(vec3 lightDirection, vec3 viewDirection, vec3 specularColor);
float Attenuation(vec3 lightPosition, float linear, float quadratic);

void main()
{
    vec3 diffuseMap = vec3(0.0);
    vec3 specularMap = vec3(0.0);

    // Get Textures
    for (uint i = 0; i < u_Material.DiffuseCount; ++i)
        diffuseMap += GetTexture(u_Material.DiffuseMaps[i]);
    for (uint i = 0; i < u_Material.SpecularCount; ++i)
        specularMap += GetTexture(u_Material.SpecularMaps[i]);

    // Lighting
    vec3 viewDirection = normalize(u_ViewPosition - Vertex.Position);
    vec3 pointLighting = PointLights(diffuseMap.rgb, specularMap.rgb, viewDirection);
    vec3 directionalLighting = DirectionalLights(diffuseMap.rgb, specularMap.rgb, viewDirection);
    vec3 spotLighting = SpotLights(diffuseMap.rgb, specularMap.rgb, viewDirection);
    FragColor = vec4(pointLighting + directionalLighting + spotLighting, 1.0);

    // Emission Map
    // vec4 emissionMap = vec4(0.0);
    // for (uint i = 0; i < u_Material.EmissionCount; ++i)
    //     emissionMap += GetTexture(u_Material.EmissionMaps[i]);
    // FragColor += emissionMap;
}

vec3 GetTexture(sampler2D textureSample)
{
    vec4 tex = texture(textureSample, Vertex.UV);
    return tex.rgb * tex.a;
}

vec3 PointLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection)
{
    vec3 diffuse = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 specular = vec3(0.0);
    for (uint i = 0; i < Light.PointCount; ++i)
    {
        PointLight light = Light.PointLights[i];
        vec3 lightDirection = normalize(light.Position - Vertex.Position);
        vec3 diffuseLighting = DiffuseLighting(lightDirection, light.BaseColor);
        vec3 specularLighting = SpecularLighting(lightDirection, viewDirection, light.SpecularColor);
        float attenuation = Attenuation(light.Position, light.Linear, light.Quadratic);

        diffuse += diffuseLighting * attenuation * light.Intensity;
        ambient += light.AmbientColor * light.Intensity * AMBIENT_INTENSITY_PERCENTAGE;
        specular +=  specularLighting * attenuation * light.Intensity;
    }
    diffuse *= diffuseMap;
    ambient *= diffuseMap;
    specular *= specularMap;
    return diffuse + ambient + specular;
}

vec3 DirectionalLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection)
{
    vec3 diffuse = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 specular = vec3(0.0);
    for (uint i = 0; i < Light.DirectionalCount; ++i)
    {
        DirectionalLight light = Light.DirectionalLights[i];
        vec3 lightDirection = normalize(-light.Direction);
        vec3 diffuseLighting = DiffuseLighting(lightDirection, light.BaseColor);
        vec3 specularLighting = SpecularLighting(lightDirection, viewDirection, light.SpecularColor);

        diffuse += diffuseLighting *  light.Intensity;
        ambient += light.AmbientColor * light.Intensity * AMBIENT_INTENSITY_PERCENTAGE;
        specular +=  specularLighting * light.Intensity;
    }
    diffuse *= diffuseMap;
    ambient *= diffuseMap;
    specular *= specularMap;
    return diffuse + ambient + specular;
}

vec3 SpotLights(vec3 diffuseMap, vec3 specularMap, vec3 viewDirection)
{
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 ambient = vec3(0.0);
    for (uint i = 0; i < Light.SpotCount; ++i)
    {
        SpotLight light = Light.SpotLights[i];
        vec3 lightDirection = normalize(light.Position - Vertex.Position);

        float cutOff = cos(light.CutOff);
        float outerCutOff = cos(light.OuterCutOff);
        float theta = dot(lightDirection, normalize(-light.Direction));
        float epsilon = cutOff - outerCutOff;
        float cutOffIntensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

        vec3 diffuseLighting = DiffuseLighting(lightDirection, light.BaseColor);
        vec3 specularLighting = SpecularLighting(lightDirection, viewDirection, light.SpecularColor);
        float attenuation = Attenuation(light.Position, light.Linear, light.Quadratic);

        diffuse += diffuseLighting * attenuation * light.Intensity * cutOffIntensity;
        ambient += light.AmbientColor * light.Intensity * AMBIENT_INTENSITY_PERCENTAGE;
        specular +=  specularLighting * attenuation * light.Intensity * cutOffIntensity;
    }
    diffuse *= diffuseMap;
    ambient *= diffuseMap;
    specular *= specularMap;
    return diffuse + ambient + specular;
}

vec3 DiffuseLighting(vec3 lightDirection, vec3 lightColor)
{
    float diffuse = clamp(dot(Vertex.Normal, lightDirection), 0.0, 1.0);
    return lightColor * diffuse;
}

vec3 SpecularLighting(vec3 lightDirection, vec3 viewDirection, vec3 specularColor)
{
    vec3 reflectDirection = reflect(-lightDirection, Vertex.Normal);
    float specular = pow(clamp(dot(viewDirection, reflectDirection), 0.0, 1.0), 32);
    return specular * specularColor;
}

float Attenuation(vec3 lightPosition, float linear, float quadratic)
{
    float lightDistance = distance(lightPosition, Vertex.Position);
    return 1.0 / (1.0 + linear * lightDistance + quadratic * (lightDistance * lightDistance));
}
