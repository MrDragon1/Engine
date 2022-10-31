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

in vec2 v_TexCoord;

layout(rgba16f) uniform image2D o_image;
layout(rgba16f) uniform image2D i_image;
layout(rgba16f) uniform image2D i_DownSamplerImage;

uniform int u_DownSample;
uniform int u_MipLevel;
uniform float u_Threshold;
uniform float u_Knee;

// Quadratic color thresholding
// curve = (threshold - knee, knee * 2, 0.25 / knee)
vec3 QuadraticThreshold(vec3 color, float threshold, float knee)
{
    vec3 curve = vec3(threshold - knee, knee * 2, 0.25 / knee);
    // Maximum pixel brightness
    float brightness = max(max(color.r, color.g), color.b);
    // Quadratic curve
    float rq = clamp(brightness - curve.x, 0.0, curve.y);
    rq = (rq * rq) * curve.z;
    color *= max(rq, brightness - threshold) / max(brightness, 1e-6);
    return color;
}

vec3 Prefilter(vec3 color)
{
    float clampValue = 20.0f;
    color = clamp(color, vec3(0.0f), vec3(clampValue));
    color = QuadraticThreshold(color, u_Threshold, u_Knee);
    return color;
}

vec3 FirstDownsample(ivec2 uv, int texelSize)
{
    // Center
    float w[13] = float[](0.5, 0.5, 0.5, 0.5, 0.5, 0.125, 0.25, 0.125, 0.25, 0.125, 0.25, 0.125, 0.25);

    vec3 A = imageLoad(i_image, uv).rgb;// 1/8
    w[0] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), A);
    // Inner box 0.125
    vec3 B = imageLoad(i_image, uv + texelSize * ivec2(-1, -1)).rgb;// 1/8
    w[1] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), B);
    vec3 C = imageLoad(i_image, uv + texelSize * ivec2(-1, 1)).rgb;// 1/8
    w[2] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), C);
    vec3 D = imageLoad(i_image, uv + texelSize * ivec2(1, 1)).rgb;// 1/8
    w[3] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), D);
    vec3 E = imageLoad(i_image, uv + texelSize * ivec2(1, -1)).rgb;// 1/8
    w[4] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), E);

    // Outer box
    vec3 F = imageLoad(i_image, uv + texelSize * ivec2(-2, -2)).rgb;// 1/32
    w[5] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), F);
    vec3 G = imageLoad(i_image, uv + texelSize * ivec2(-2, 0)).rgb;// 1/16
    w[6] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), G);
    vec3 H = imageLoad(i_image, uv + texelSize * ivec2(-2, 2)).rgb;// 1/32
    w[7] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), H);
    vec3 I = imageLoad(i_image, uv + texelSize * ivec2(0, 2)).rgb;// 1/16
    w[8] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), I);
    vec3 J = imageLoad(i_image, uv + texelSize * ivec2(2, 2)).rgb;// 1/32
    w[9] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), J);
    vec3 K = imageLoad(i_image, uv + texelSize * ivec2(2, 0)).rgb;// 1/16
    w[10] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), K);
    vec3 L = imageLoad(i_image, uv + texelSize * ivec2(2, -2)).rgb;// 1/32
    w[11] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), L);
    vec3 M = imageLoad(i_image, uv + texelSize * ivec2(0, -2)).rgb;// 1/16
    w[12] /= 1 + dot(vec3(0.2126, 0.7152, 0.0722), M);

    // Weights
    vec3 result = vec3(0.0);
    result += A * w[0];
    result += B * w[1] + C * w[2] + D * w[3] + E * w[4];
    result += F * w[5] + G * w[6] + H * w[7] + I * w[8] + J * w[9] + K * w[10] + L * w[11] + M * w[12];

    float sum = 0.0;
    for (int i =0;i<13;i++) sum += w[i];
    result /= sum;
    // 4 samples each
    result *= 0.25f;
    return result;
}

vec3 Downsample(ivec2 uv, int texelSize)
{
    // Center
    vec3 A = imageLoad(i_image, uv).rgb;

    // Inner box
    vec3 B = imageLoad(i_image, uv + texelSize * ivec2(-1, -1)).rgb;
    vec3 C = imageLoad(i_image, uv + texelSize * ivec2(-1, 1)).rgb;
    vec3 D = imageLoad(i_image, uv + texelSize * ivec2(1, 1)).rgb;
    vec3 E = imageLoad(i_image, uv + texelSize * ivec2(1, -1)).rgb;

    // Outer box
    vec3 F = imageLoad(i_image, uv + texelSize * ivec2(-2, -2)).rgb;
    vec3 G = imageLoad(i_image, uv + texelSize * ivec2(-2, 0)).rgb;
    vec3 H = imageLoad(i_image, uv + texelSize * ivec2(-2, 2)).rgb;
    vec3 I = imageLoad(i_image, uv + texelSize * ivec2(0, 2)).rgb;
    vec3 J = imageLoad(i_image, uv + texelSize * ivec2(2, 2)).rgb;
    vec3 K = imageLoad(i_image, uv + texelSize * ivec2(2, 0)).rgb;
    vec3 L = imageLoad(i_image, uv + texelSize * ivec2(2, -2)).rgb;
    vec3 M = imageLoad(i_image, uv + texelSize * ivec2(0, -2)).rgb;

    // Weights
    vec3 result = vec3(0.0);
    // Inner box
    result += (B + C + D + E) * 0.5f;
    // Bottom-left box
    result += (F + G + A + M) * 0.125f;
    // Top-left box
    result += (G + H + I + A) * 0.125f;
    // Top-right box
    result += (A + I + J + K) * 0.125f;
    // Bottom-right box
    result += (M + A + K + L) * 0.125f;

    // 4 samples each
    result *= 0.25f;

    return result;
}

vec3 Blur1(ivec2 coords, int radius)
{
    ivec4 offset = ivec4(1, 1, -1, 0) * radius;

    // Center
    vec3 result = imageLoad(i_image, coords).rgb * 4.0f;

    result += imageLoad(i_image, coords - offset.xy).rgb;
    result += imageLoad(i_image, coords - offset.wy).rgb * 2.0;
    result += imageLoad(i_image, coords - offset.zy).rgb;

    result += imageLoad(i_image, coords + offset.zw).rgb * 2.0;
    result += imageLoad(i_image, coords + offset.xw).rgb * 2.0;

    result += imageLoad(i_image, coords + offset.zy).rgb;
    result += imageLoad(i_image, coords + offset.wy).rgb * 2.0;
    result += imageLoad(i_image, coords + offset.xy).rgb;

    return result / 16.0;
}

vec3 Blur2(ivec2 coords, int radius)
{
    ivec4 offset = ivec4(1, 1, -1, 0) * radius;

    // Center
    vec3 result = imageLoad(i_DownSamplerImage, coords).rgb * 4.0f;

    result += imageLoad(i_DownSamplerImage, coords - offset.xy).rgb;
    result += imageLoad(i_DownSamplerImage, coords - offset.wy).rgb * 2.0;
    result += imageLoad(i_DownSamplerImage, coords - offset.zy).rgb;

    result += imageLoad(i_DownSamplerImage, coords + offset.zw).rgb * 2.0;
    result += imageLoad(i_DownSamplerImage, coords + offset.xw).rgb * 2.0;

    result += imageLoad(i_DownSamplerImage, coords + offset.zy).rgb;
    result += imageLoad(i_DownSamplerImage, coords + offset.wy).rgb * 2.0;
    result += imageLoad(i_DownSamplerImage, coords + offset.xy).rgb;

    return result / 16.0;
}

void main()
{
    vec2 imgSize = vec2(imageSize(o_image));
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec3 result = vec3(1.0f, 0.0f, 0.0f);

    if (bool(u_DownSample))
    {
        if (int(u_MipLevel) == 0)
        {
            result.rgb = FirstDownsample(coords, 1);
            result.rgb = Prefilter(result);
        }
        else {
            result.rgb = Downsample(coords*2, 1);
        }
    }
    else {
        vec3 upsampledTexture = Blur1(coords/2, 1);
        vec3 existing = Blur2(coords, 1);
        result.rgb = existing + upsampledTexture;
    }
    imageStore(o_image, coords, vec4(result, 1.0));
} 