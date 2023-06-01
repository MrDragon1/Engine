void HanrahanKrueger_subsurface(vec3 color, float roughness, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = dot(L,H);
    float NdotL = dot(N,L);
    float NdotV = dot(N,V);
    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fss90 = LdotH * LdotH * roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1 / (NdotL + NdotV) - .5) + .5);
    out1.response = ss * color * PI_INV;
}
