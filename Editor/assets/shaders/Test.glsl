// Basic Texture Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in int a_EntityID;

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
    v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Color;
in vec3 v_Normal;
in vec2 v_TexCoord;
in flat int v_EntityID;

void main()
{
    color = v_Color;
    color2 = v_EntityID;
}