#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

layout(location = 0) in vec3 v_LocalPos;
uniform samplerCube u_SkyboxTexture;

void main()
{
    vec3 envColor = texture(u_SkyboxTexture, v_LocalPos).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 

    FragColor = vec4(envColor, 1.0);
    EntityID = -1;
}