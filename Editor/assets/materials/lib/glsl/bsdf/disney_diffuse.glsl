void disney_diffuse(vec3 color, float roughness, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = clamp(dot(L,H), FLOAT_EPS, 1.0);
    float NdotL = clamp(dot(N,L), FLOAT_EPS, 1.0);
    float NdotV = clamp(dot(N,V), FLOAT_EPS, 1.0);

    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fd90 = 0.5 + 2 * LdotH * LdotH * roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);
    out1.response = Fd * color * PI_INV;
}
