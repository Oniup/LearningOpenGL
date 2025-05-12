#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

layout (std140, binding = 0) uniform Matrices
{
    mat4 u_Projection;
    mat4 u_View;
};

uniform mat4 u_Model;

out VS_VERTEX
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} Vertex;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);

    Vertex.Position = vec3(u_Model * vec4(a_Position, 1.0));
    Vertex.Normal = normalize(mat3(transpose(inverse(u_Model))) * a_Normal);
    Vertex.UV = a_UV;
}
