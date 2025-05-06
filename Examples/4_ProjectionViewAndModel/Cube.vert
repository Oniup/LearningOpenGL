#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal; // Not using
layout (location = 2) in vec2 a_UV;

out vec2 UV;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position.xyz, 1.0);
    UV = a_UV;
}
