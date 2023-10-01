#version 460 core
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) in vec3 a_Position;

layout(std140,set=0, binding = 0) uniform Config{
        mat4 Projection; 
        mat4 View;
        float Roughness; 
}u_Config;

layout(location = 0) out vec3 v_WorldPos;

void main()
{
    v_WorldPos = a_Position;
    gl_Position = u_Config.Projection * u_Config.View * vec4(v_WorldPos, 1.0);
}