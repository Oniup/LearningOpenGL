#version 450 core

#define MAX_TEXTURE_COUNT 8

struct Material
{
    int DiffuseMapCount;
    int SpecularMapCount;
    sampler2D DiffuseMap[MAX_TEXTURE_COUNT];
    sampler2D SpecularMap[MAX_TEXTURE_COUNT];
};

in VS_VERTEX
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} Vertex;

uniform Material u_Material;

out vec4 FragColor;

void main()
{
//    vec4 diffuseColor;
//    for (int i = 0; i < u_Material.DiffuseMapCount; ++i)
//        diffuseColor += texture(u_Material.DiffuseMap[i], Vertex.UV);

    FragColor = vec4(0.2, 0.5, 0.8, 1.0);
}
