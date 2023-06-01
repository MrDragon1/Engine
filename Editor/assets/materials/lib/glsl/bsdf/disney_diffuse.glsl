void disney_diffuse(vec3 color, float roughness, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = dot(L,H);
    float NdotL = dot(N,L);
    float NdotV = dot(N,V);

    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fd90 = 0.5 + 2 * LdotH * LdotH * roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);
    out1.response = Fd * color * PI_INV;
}
