#version 450 core

in vec3 VertColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(VertColor.xyz, 1.0);
}
