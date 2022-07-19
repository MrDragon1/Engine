// Shadow Map Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 v_LocalPos;

void main()
{
    v_LocalPos = a_Position;

    mat4 rotView = mat4(mat3(u_View)); // remove translation from the view matrix
    vec4 clipPos = u_Projection * rotView * vec4(v_LocalPos, 1.0);
    gl_Position = clipPos.xyww;
}

#type fragment
#version 460 core
out vec4 FragColor;
in vec3 v_LocalPos;
uniform samplerCube u_SkyboxTexture;

void main()
{
    vec3 envColor = texture(u_SkyboxTexture, v_LocalPos).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 

    FragColor = vec4(envColor, 1.0);
}