// Pixel shader combines the bloom image with the original
// scene, using tweakable intensity levels and saturation.
// This is the final step in applying a bloom postprocess.

static const float3 GrayScaleIntensity = { 0.299f, 0.587f, 0.114f };

Texture2D BloomTexture : register(t0);
Texture2D SceneTexture : register(t1);

SamplerState TrilinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

cbuffer cbPerFrame : register(b0)
{
  /*  float BloomIntensity :  packoffset(c0.x);
    float BaseIntensity :   packoffset(c0.y);

    float BloomSaturation : packoffset(c0.z);
    float BaseSaturation :  packoffset(c0.w);*/

	float BloomIntensity;
	float BloomSaturation;
	float SceneIntensity;
	float SceneSaturation;
}

// Helper for modifying the saturation of a color.
float4 AdjustSaturation(float4 color, float saturation)
{
    // The constants 0.3, 0.59, and 0.11 are chosen because the
    // human eye is more sensitive to green light, and less to blue.
    float grey = dot((float3)color, GrayScaleIntensity);

    return lerp(grey, color, saturation);
}

// Defines the output data of our vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	// Look up the bloom and original base image colors.
	float4 bloom = BloomTexture.Sample(TrilinearSampler, input.uv);
	float4 base = SceneTexture.Sample(TrilinearSampler, input.uv);

    // Adjust color saturation and intensity.
    bloom = AdjustSaturation(bloom, BloomSaturation) * BloomIntensity;
    base = AdjustSaturation(base, SceneSaturation) * SceneIntensity;

    // Darken down the base image in areas where there is a lot of bloom,
    // to prevent things looking excessively burned-out.
    base *= (1 - saturate(bloom));

    // Combine the two images.
    return base + bloom;
}