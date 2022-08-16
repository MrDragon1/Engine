// Merge Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}

    #type fragment
    #version 460 core

//layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform layout(rgba16f) image2D o_image;
uniform layout(rgba16f) image2D i_image;
uniform sampler2D u_DownSamplerImage;
uniform int u_DownSample;
uniform int u_FirstUpSample;
uniform int u_MipLevel;


float GaussianWeight(float x, float y, float sigma)
{
    float PI = 3.1415926;
    float E = 2.71828182826;
    float sigma_2 = pow(sigma, 2.0);

    float a = -(x*x+y*y) / (2.0 * sigma_2);
    return pow(E, a) / (2.0 * PI * sigma_2);

}

vec3 DownsampleBox13(sampler2D tex, float lod, vec2 uv, vec2 texelSize)
{
    texelSize *= 0.5f;// Sample from center of texels
    float weight = 0.0;
    vec3 result = vec3(0.0);
    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            float w = GaussianWeight(i, j, 4.0);
            result += textureLod(tex, uv + vec2(texelSize.x * i, texelSize.y * j), lod).rgb * w;
            weight += w;
        }
    }
    return result / weight;
}
vec3 UpsampleTent9(sampler2D tex, float lod, vec2 uv, vec2 texelSize, float radius)
{
    texelSize *= 0.5f;
    float weight = 0.0;
    vec3 result = vec3(0.0);
    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            float w = GaussianWeight(i, j, 4.0);
            result += textureLod(tex, uv + vec2(texelSize.x * i * radius, texelSize.y * j * radius), lod).rgb * w;
            weight += w;
        }
    }
    return result / weight;
}

vec3 Blur(ivec2 coords)
{
    float weight = 0.0;
    vec3 result = vec3(0.0);
    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            float w = GaussianWeight(i, j, 4.0);
            result += imageLoad(i_image, coords + ivec2(i, j)).rgb * w;
            weight += w;
        }
    }
    return result / weight;
}

void main()
{
    vec2 imgSize = vec2(imageSize(o_image));
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec3 result = vec3(1.0f, 0.0f, 0.0f);
    vec2 texCoords = vec2(float(coords.x) / imgSize.x, float(coords.y) / imgSize.y);
    texCoords += (1.0f / imgSize) * 0.5f;

    if (bool(u_DownSample))
    {
        vec2 texSize = vec2(textureSize(u_DownSamplerImage, int(u_MipLevel)));
        if (int(u_MipLevel) == 0)
        {
            result.rgb = texture(u_DownSamplerImage, texCoords).rgb;
            //result = Prefilter(result, texCoords);
        }
        else {
            result.rgb = DownsampleBox13(u_DownSamplerImage, u_MipLevel - 1.0f, texCoords, 1.0f / texSize);
        }
    }
    else {
        if (bool(u_FirstUpSample))
        {
            vec2 bloomTexSize = imgSize / 2.0;
            float sampleScale = 1.0f;
            vec3 upsampledTexture = UpsampleTent9(u_DownSamplerImage, u_MipLevel + 1.0f, texCoords, 1.0f / bloomTexSize, sampleScale);
            vec3 existing = UpsampleTent9(u_DownSamplerImage, u_MipLevel, texCoords, 1.0f / imgSize, sampleScale);

            result.rgb = existing + upsampledTexture;
        }
        else {
            vec2 bloomTexSize = imgSize / 2.0;
            float sampleScale = 1.0f;

            vec3 upsampledTexture = Blur(coords/2);
            vec3 existing = UpsampleTent9(u_DownSamplerImage, u_MipLevel, texCoords, 1.0f / imgSize, sampleScale);

            result.rgb = existing + upsampledTexture;
        }
    }
    imageStore(o_image, coords, vec4(result, 1.0));
} 