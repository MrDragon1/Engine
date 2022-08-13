// Merge Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}

    #type fragment
    #version 460 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;


uniform sampler2D u_BrightImage;
uniform float weight[16] = float[] (0.0794977567272904, 0.0763806050998811, 0.0705081851146027, 0.0625351503339776, 0.0532889399491673, 0.0436292939353091, 0.0343200181681762, 0.0259385117379726, 0.0188352253216535, 0.0131408908031312, 0.00880860252810425, 0.00567306084694417, 0.00351039583279473, 0.00208700245410541, 0.00119211471805973, 0.000654246428830130);
uniform bool u_Horizontal;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(u_BrightImage, 0);// gets size of single texel
    vec3 result = texture(u_BrightImage, v_TexCoord).rgb * weight[0];
    if (u_Horizontal)
    {
        for (int i = 1; i < 16; ++i)
        {
            result += texture(u_BrightImage, v_TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_BrightImage, v_TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 16; ++i)
        {
            result += texture(u_BrightImage, v_TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(u_BrightImage, v_TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    color = vec4(result, 1.0);
} 