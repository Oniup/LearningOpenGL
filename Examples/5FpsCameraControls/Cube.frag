#version 450 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D u_Diffuse;

void main()
{
    FragColor = texture(u_Diffuse, UV);
}
