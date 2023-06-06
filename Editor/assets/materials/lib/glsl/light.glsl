void mx_directional_light(LightData light, vec3 position, out lightshader result)
{
    result.direction = light.direction;
    result.intensity = light.color * light.intensity;
}

void mx_point_light(LightData light, vec3 position, out lightshader result)
{
    result.direction = light.direction - position;
    result.intensity = light.color * light.intensity;
}

int numActiveLightSources()
{
    return min(u_numActiveLightSources, MAX_LIGHT_SOURCES) ;
}

void sampleLightSource(LightData light, vec3 position, out lightshader result)
{
    result.intensity = vec3(0.0);
    result.direction = vec3(0.0);
    if (light.type == 0)
    {
        mx_directional_light(light, position, result);
    }
    else if (light.type == 1)
    {
        mx_point_light(light, position, result);
    }
}
