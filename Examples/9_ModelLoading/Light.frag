#version 450 core

out vec4 FragColor;

uniform vec3 u_LightBaseColor;

void main()
{
    FragColor = vec4(u_LightBaseColor, 1.0);
}
