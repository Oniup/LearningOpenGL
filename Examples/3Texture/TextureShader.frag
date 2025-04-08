#version 450 core

in vec2 UV;
out vec4 FragColor;

uniform bool u_UseMix;
uniform sampler2D u_Diffuse;
uniform sampler2D u_SmileFace;

void main()
{
    vec4 diffuseTex = texture(u_Diffuse, UV);
    vec4 smileFaceTex = texture(u_SmileFace, UV);
    if (u_UseMix == true)
        FragColor = mix(diffuseTex, smileFaceTex, 0.2);
    else
    {
        if (smileFaceTex.a > 0.0)
            FragColor = smileFaceTex;
        else
            FragColor = diffuseTex;
    }
}
