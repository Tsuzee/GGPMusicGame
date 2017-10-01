#define SAMPLE_COUNT 9

// Defines the output data of our vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

Texture2D InputTexture : register(t0);

cbuffer CBufferPerFrame : register(b0)
{
	float4 SampleOffsets[SAMPLE_COUNT];
	float4 SampleWeights[SAMPLE_COUNT];

	//float4 AggressiveSampleOffsets[5];
	//float4 AggressiveSampleWeights[3];
}

SamplerState TrilinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

float4 main(VertexToPixel input) : SV_Target
{
	float4 color = (float4)0;

	//static float2 SampleOffsets[10] = (float2[10])AggressiveSampleOffsets;
	//static float SampleWeights[12] = (float[12])AggressiveSampleWeights;

	for (int i = 0; i < SAMPLE_COUNT; i++)
	{
		color += InputTexture.Sample(TrilinearSampler,
			input.uv + SampleOffsets[i].xy) * SampleWeights[i].x;
	}
	return color;
}
