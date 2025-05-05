#version 450 core

const int MaxLightCount = 10;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

struct Material
{
    sampler2D DiffuseMap;
    sampler2D SpecularMap;
    float Shininess;
};

uniform int u_LightCount;
uniform Light u_Lights[MaxLightCount];

uniform vec3 u_AmbientLight;
uniform vec3 u_ViewPosition;

uniform Material u_Material;

in VS_VERTEX 
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} Vertex;

out vec4 FragColor;

vec3 DiffuseLighting(vec3 lightDirection, vec3 lightColor)
{
    float diff = max(dot(Vertex.Normal, lightDirection), 0.0);
    return diff * lightColor;
}

vec3 SpecularLighting(vec3 lightDirecftion, vec3 lightColor)
{
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(u_ViewPosition - Vertex.Position);
    vec3 reflectDirection = reflect(-lightDirecftion, Vertex.Normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), u_Material.Shininess);
    return specularStrength * spec * lightColor;
}

float CalcAttenuation(Light light)
{
    float dist = length(light.Position - Vertex.Position);
    float atten = 1.0 / (1.0 + light.Linear * dist + light.Quadratic * (dist * dist));;
    return atten;
}

void main()
{
    vec4 diffuseMap = texture(u_Material.DiffuseMap, Vertex.UV);
    if (diffuseMap.a < 0.05)
        discard;
    vec4 specularMap = texture(u_Material.SpecularMap, Vertex.UV);

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < u_LightCount; ++i)
    {
        vec3 lightDirection = normalize(u_Lights[i].Position - Vertex.Position);
        vec3 lightColor = u_Lights[i].Color;

        float attenuation = CalcAttenuation(u_Lights[i]);

        vec3 ambientLighting = u_AmbientLight;
        vec3 diffuseLighting = DiffuseLighting(lightDirection, lightColor);
        vec3 specularLighting = SpecularLighting(lightDirection, lightColor);

        ambientLighting *= attenuation;
        diffuseLighting *= attenuation;
        specularLighting *= attenuation;

        ambient += ambientLighting;
        diffuse += diffuseLighting * diffuseMap.xyz;
        specular += specularLighting * specularMap.xyz;
    }

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
