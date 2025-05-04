#version 450 core

const int MaxLightCount = 10;

struct Light
{
    vec3 Position;
    vec3 Color;
};

uniform int u_LightCount;
uniform Light u_Lights[MaxLightCount];

uniform vec3 u_AmbientLight;
uniform sampler2D u_Diffuse;
uniform vec3 u_ViewPosition;

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
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    return specularStrength * spec * lightColor;
}

void main()
{
    vec4 diffuseColor = texture(u_Diffuse, Vertex.UV);

    vec3 lightsColor;
    for (int i = 0; i < u_LightCount; ++i)
    {
        vec3 lightDirection = normalize(u_Lights[i].Position - Vertex.Position);
        vec3 lightColor = u_Lights[i].Color;

        vec3 diffuseLighting = DiffuseLighting(lightDirection, lightColor);
        vec3 specularLighting = SpecularLighting(lightDirection, lightColor);

        lightsColor += diffuseLighting + specularLighting;
    }

    FragColor = vec4(u_AmbientLight + lightsColor, 1.0) * diffuseColor;
}
