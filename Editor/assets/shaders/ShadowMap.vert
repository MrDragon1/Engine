#version 460 core

layout(location = 0) in vec3 a_Position;

layout(binding = 0) uniform UBO{
    mat4 u_ViewProjection;
     mat4 u_Model;
}ubo;

void main()
{
    gl_Position =  ubo.u_ViewProjection * ubo.u_Model * vec4(a_Position, 1.0);
}
