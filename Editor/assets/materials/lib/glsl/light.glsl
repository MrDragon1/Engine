
// vec3 mx_environment_radiance(vec3 N, vec3 V, float roughness, vec3 color){
//     float NoV = dot(N,V);
//     NoV = clamp(NoV, 0.0, 1.0);
// 
//     vec3 Lr = 2.0 * NoV * N - V;
//     vec3 F0 = mix(vec3(0.04), color, metallic);
// 
//     int envRadianceTexLevels = textureQueryLevels(u_IrradianceMap);
//     vec3 specularIrradiance = textureLod(u_PrefilterMap, Lr, (roughness) * envRadianceTexLevels).rgb;
//     vec2 specularBRDF = texture(u_BRDFLUT, vec2(NoV, 1.0 - roughness)).rg;
//     return specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);
// }
// 
// vec3 mx_environment_irradiance(vec3 N){
//     return texture(u_IrradianceMap, N).rgb;
// }

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
