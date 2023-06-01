void surface(BSDF bsdf, float opacity, inout surfaceshader result)
{
    result.color = bsdf.response;
    result.transparency = vec3(opacity);
}
