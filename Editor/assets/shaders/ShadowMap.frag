#version 460 core

float near = 0.1;
float far  = 7.5;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    gl_FragDepth = gl_FragCoord.z;
}