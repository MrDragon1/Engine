void disney_sheen(vec3 color, float sheen, float sheenTint, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = clamp(dot(L,H), FLOAT_EPS, 1.0);
    float FH = SchlickFresnel(LdotH);
    vec3 Csheen = mix(vec3(1), color, sheenTint);
    out1.response = FH * sheen * Csheen;
}
