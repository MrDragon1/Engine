// Shadow Map Shader

#type vertex
#version 460 core
layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjectionMatrix;
    mat4 InverseViewProjectionMatrix;
    mat4 ProjectionMatrix;
    mat4 InverseProjectionMatrix;
    mat4 ViewMatrix;
    mat4 InverseViewMatrix;
    vec2 NDCToViewMul;
    vec2 NDCToViewAdd;
    vec2 DepthUnpackConsts;
    vec2 CameraTanHalfFOV;
} u_Camera;
layout(location = 0) in vec3 a_Position;

out vec3 v_LocalPos;

void main()
{
    v_LocalPos = a_Position;

    mat4 rotView = mat4(mat3(u_Camera.ViewMatrix)); // remove translation from the view matrix
    vec4 clipPos = u_Camera.ProjectionMatrix * rotView * vec4(v_LocalPos, 1.0);
    gl_Position = clipPos.xyww;
}

#type fragment
#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

in vec3 v_LocalPos;
uniform samplerCube u_SkyboxTexture;

void main()
{
    vec3 envColor = texture(u_SkyboxTexture, v_LocalPos).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 

    FragColor = vec4(envColor, 1.0);
    EntityID = -1;
}