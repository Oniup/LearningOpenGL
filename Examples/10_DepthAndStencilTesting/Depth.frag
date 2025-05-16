#version 450 core

out vec4 FragColor;

void main()
{
    float near = 0.1;
    float far = 100.0;
    float z = gl_FragCoord.z * 2.0 - 1.0;
    float depth = ((2.0 * near * far) / (far + near - z * (far - near))) / far;
    FragColor = vec4(vec3(depth), 1.0);
}
