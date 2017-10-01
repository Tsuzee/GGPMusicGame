static const float3 GrayScaleIntensity = { 0.299f, 0.587f, 0.114f };

Texture2D InputTexture : register(t0);

cbuffer CBufferPerObject : register(b0)
{
	float BloomThreshold;
	//float BloomIntensity = 1.25f;
	//float BloomSaturation = 1.0f;
	//float SceneIntensity = 1.0f;
	//float SceneSaturation = 1.0f;
};

SamplerState TrilinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// Defines the output data of our vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    // Look up the original image color.
    float4 sampledColor = InputTexture.Sample(TrilinearSampler, input.uv);
	
	float intensity = dot(sampledColor.rgb, GrayScaleIntensity);

	return (intensity > BloomThreshold ? sampledColor : float4(0, 0, 0, 1));
    
    // Adjust it to keep only values brighter than the specified threshold. Use saturate to clamp the values
	// within unorm range.
    return float4(saturate((sampledColor.rgb - BloomThreshold) / (1 - BloomThreshold)), sampledColor.a);
}
