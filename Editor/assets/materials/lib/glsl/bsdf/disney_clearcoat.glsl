void disney_clearcoat(float clearcoat, float clearcoatGloss, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float NdotH = clamp(dot(N,H), FLOAT_EPS, 1.0);
    float LdotH = clamp(dot(L,H), FLOAT_EPS, 1.0);
    float NdotL = clamp(dot(N,L), FLOAT_EPS, 1.0);
    float NdotV = clamp(dot(N,V), FLOAT_EPS, 1.0);
    float FH = SchlickFresnel(LdotH);

    float Dr = GTR1(NdotH, mix(.1,.001,clearcoatGloss));
    float Fr = mix(.04, 1.0, FH);
    float Gr = smithG_GGX(NdotL, .25) * smithG_GGX(NdotV, .25);
    out1.response = vec3(clearcoat * Gr * Fr * Dr);
}
