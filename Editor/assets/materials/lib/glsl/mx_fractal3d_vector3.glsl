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
