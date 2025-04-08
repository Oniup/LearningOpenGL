#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

out vec3 VertColor;

void main()
{
    gl_Position = vec4(a_Position.xyz, 1.0);
    VertColor = a_Color;
}
