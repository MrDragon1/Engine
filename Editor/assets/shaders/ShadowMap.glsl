// Basic Texture Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec4 v_Color;
out vec3 v_Normal;
out vec2 v_TexCoord;
out flat int v_EntityID;

void main()
{
	v_TexCoord = a_TexCoord;
    v_Normal = a_Normal;
    v_Color = a_Color;
	gl_Position =  u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec3 v_Normal;
in vec2 v_TexCoord;

void main()
{
    int index = 0;
    color =  ;
}