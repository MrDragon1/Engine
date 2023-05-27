#version 460

struct BSDF { vec3 response; vec3 throughput; float thickness; float ior; };
struct surfaceshader { vec3 color; vec3 transparency; };
#define material surfaceshader

layout(std140, binding = 1) uniform pspubuniform
{
    vec3 param_ambient;
    float param_roughness;
    float NG_marble1_noise_diminish;
    vec3 NG_marble1_noise_amplitude;
    float NG_marble1_noise_scale_2;
    vec3 NG_marble1_base_color_2;
    float NG_marble1_noise_lacunarity;
    float NG_marble1_noise_scale_1;
    vec3 NG_marble1_base_color_1;
    float NG_marble1_noise_power;
    int NG_marble1_param_noise_octaves;
    vec3 NG_marble1_param_1;
    vec3 NG_marble1_param_2;
    float NG_marble1_param_3;
    float NG_standard_surface_surfaceshader_phong_metalness;
    vec3 NG_standard_surface_surfaceshader_phong_trans;
} VBpspubuniform;

in layout(location = 0) vertexData
{
    vec3 positionWorld;
} vd;

// Pixel Shader Output
layout(location = 0) out vec4 out1;

vec3 mx_fractal_noise_vec3(vec3 p, int octaves, float lacunarity, float diminish)
{
    vec3 result = vec3(0.0);
    float amplitude = 1.0;
    for (int i = 0;  i < octaves; ++i)
    {
        result += amplitude * p;
        amplitude *= diminish;
        p *= lacunarity;
    }
    return result;
}

void mx_fractal3d_vector3(vec3 amplitude, int octaves, float lacunarity, float diminish, vec3 position, out float result)
{
    vec3 value = mx_fractal_noise_vec3(position, octaves, lacunarity, diminish);
    result = value.x + amplitude.x;
}

void surface(vec3 color, vec3 transparency, out surfaceshader result)
{
    result.color = color;
    result.transparency = transparency;
}

void NG_standard_surface_surfaceshader_phong(vec3 base_color, vec3 ambient, float roughness, float metalness, vec3 trans, out surfaceshader out1)
{
    vec3 multiply_diffuse_out1 = base_color * roughness;
    vec3 multiply_specular_out1 = base_color * metalness;
    vec3 add_diffuse_out1 = multiply_diffuse_out1 + multiply_specular_out1;
    vec3 add_ambient_out1 = add_diffuse_out1 + ambient;
    surfaceshader shader_constructor_out1 = surfaceshader(vec3(0.0),vec3(0.0));
    surface(add_ambient_out1, trans, shader_constructor_out1);
    out1 = shader_constructor_out1;
}

void surfacematerial(surfaceshader surfaceshader, out material result)
{
    result = surfaceshader;
}

void main()
{
    float NG_marble1_add_xyz_out1 = dot(VBpspubuniform.NG_marble1_param_1, VBpspubuniform.NG_marble1_param_2);
    vec3 NG_marble1_scale_pos_out1 = VBpspubuniform.NG_marble1_param_2 * VBpspubuniform.NG_marble1_noise_scale_2;
    float NG_marble1_scale_xyz_out1 = NG_marble1_add_xyz_out1 * VBpspubuniform.NG_marble1_noise_scale_1;
    float NG_marble1_noise_out1 = float(0.0);
    mx_fractal3d_vector3(VBpspubuniform.NG_marble1_noise_amplitude, VBpspubuniform.NG_marble1_param_noise_octaves, VBpspubuniform.NG_marble1_noise_lacunarity, VBpspubuniform.NG_marble1_noise_diminish, NG_marble1_scale_pos_out1, NG_marble1_noise_out1);
    float NG_marble1_scale_noise_out1 = NG_marble1_noise_out1 * VBpspubuniform.NG_marble1_param_3;
    float NG_marble1_sum_out1 = NG_marble1_scale_xyz_out1 + NG_marble1_scale_noise_out1;
    float NG_marble1_sin_out1 = sin(NG_marble1_sum_out1);
    float NG_marble1_scale_out1 = NG_marble1_sin_out1 * VBpspubuniform.NG_marble1_param_3;
    float NG_marble1_bias_out1 = NG_marble1_scale_out1 + VBpspubuniform.NG_marble1_param_3;
    float NG_marble1_power_out1 = pow(NG_marble1_bias_out1, VBpspubuniform.NG_marble1_noise_power);
    vec3 NG_marble1_color_mix_out1 = mix(VBpspubuniform.NG_marble1_base_color_1, VBpspubuniform.NG_marble1_base_color_2, NG_marble1_power_out1);
    vec3 NG_marble1_out1 = vec3(1.0);
    NG_marble1_out1 = NG_marble1_color_mix_out1;

    surfaceshader SR_marble1_out1 = surfaceshader(vec3(0.0),vec3(0.0));
    NG_standard_surface_surfaceshader_phong(NG_marble1_out1, VBpspubuniform.param_ambient, VBpspubuniform.param_roughness, VBpspubuniform.NG_standard_surface_surfaceshader_phong_metalness, VBpspubuniform.NG_standard_surface_surfaceshader_phong_trans, SR_marble1_out1);
    material Marble_3D_out1 = material(vec3(0.0),vec3(0.0));
    surfacematerial(SR_marble1_out1, Marble_3D_out1);
    out1 = vec4(Marble_3D_out1.color, 1.0);
}