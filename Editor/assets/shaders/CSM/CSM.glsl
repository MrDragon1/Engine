// Cascaded Shadow Map Shader

#type vertex
#version 460 core
layout(location = 0) in vec3 a_Position;
uniform mat4 u_Model;
void main()
{
    gl_Position = u_Model * vec4(a_Position, 1.0);
}
    #type geometry
    #version 460 core
layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;


layout (std140, binding = 1) uniform ShadowData
{
    int CascadeCount;
    float CascadeSplits[16];
    mat4 DirLightMatrices[16];
} u_CascadeShadowData;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = u_CascadeShadowData.DirLightMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}

    #type fragment
    #version 460 core
void main()
{
}