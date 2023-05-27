#version 460

layout(std140, binding = 0) uniform vspubuniform
{
    mat4 u_modelMatrix;
    mat4 u_viewMatrix;
    mat4 u_projectionMatrix;
} VBvspubuniform;

layout(location = 0 ) in vec3 i_position;
layout(location = 1 ) in vec3 i_normal;

out layout(location = 0) vertexData
{
    vec3 positionWorld;
} vd;

void main()
{
    vec4 hPositionWorld = VBvspubuniform.u_modelMatrix * vec4(i_position, 1.0);
    gl_Position = VBvspubuniform.u_projectionMatrix * VBvspubuniform.u_viewMatrix * hPositionWorld;
    vd.positionWorld = hPositionWorld.xyz;
}