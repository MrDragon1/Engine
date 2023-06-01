void disney_specular(vec3 color, float metallic, float specular, float roughness, float specularTint, float anisotropic, vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float NdotH = dot(N,H);
    float LdotH = dot(L,H);
    float NdotV = dot(N,V);
    float NdotL = dot(N,L);
    
    float aspect = sqrt(1-anisotropic*.9);
    float ax = max(.001, sqr(roughness)/aspect);
    float ay = max(.001, sqr(roughness)*aspect);
    float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
    float FH = SchlickFresnel(LdotH);
    vec3 Fs = mix(color, vec3(1), FH);
    float Gs;
    Gs  = smithG_GGX_aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
    Gs *= smithG_GGX_aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);
    out1.response = Gs * Fs * Ds;
}
