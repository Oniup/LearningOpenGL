#version 450 core

#define MAX_TEXTURE_MAPS 4

struct Material
{
    uint DiffuseCount;
    uint SpecularCount;
    uint EmissionCount;
    sampler2D DiffuseMaps[MAX_TEXTURE_MAPS];
    sampler2D SpecularMaps[MAX_TEXTURE_MAPS];
    sampler2D EmissionMaps[MAX_TEXTURE_MAPS];
};

in VS_VERTEX
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} Vertex;

out vec4 FragColor;

uniform Material u_Material;

void main()
{
    vec4 diffuseMap = vec4(0.0);
    for (uint i = 0; i < u_Material.DiffuseCount; ++i)
        diffuseMap += texture(u_Material.DiffuseMaps[i], Vertex.UV);

    FragColor = diffuseMap;
//    FragColor = vec4(0.2);
}
