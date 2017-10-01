// Base provided by Chris Cascioli
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: WORLDPOS;
	float2 uv			: TEXCOORD;
};

struct DirectionalLight
{
	float4 DiffuseColor;
	float3 Direction;
};

struct PointLight
{
	float4 DiffuseColor;
	float3 Position;
};

struct SpotLight
{
	float4 DiffuseColor;
	float3 Position;
	float theta;
	float3 Direction;
	float phi;
};

struct GlobalLight
{
	float4 AmbientColor;
};

cbuffer lightData : register(b0)
{
	GlobalLight ambient0;
	float4 camPos;
	DirectionalLight light0;
	DirectionalLight light1;
	PointLight lightP0;
	SpotLight lightS0;
};

Texture2D diffuseTexture : register(t0);
Texture2D NormalMap : register(t1);
TextureCube Sky			: register(t2);
SamplerState basicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);


	//---------------------------------------------------------------------------------------------
	//Normal Mapping

	//Read and unpack the normal from the map
	float3 normalFromMap = NormalMap.Sample(basicSampler, input.uv).xyz * 2 - 1;

	//Transform from tangent to world space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);

	float3x3 TBN = float3x3(T, B, N);
	input.normal = normalize(mul(normalFromMap, TBN));

	//Texture
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	// Alpha clipping: Discard the entire pixel if alpha goes below 25%
	//clip(surfaceColor.a - 0.25f);

	//---------------------------------------------------------------------------------------------
	//Global Light(s)
	float4 global = surfaceColor * ambient0.AmbientColor;

	//---------------------------------------------------------------------------------------------
	//Directional Light(s)
	float3 lightNorm = normalize(light0.Direction);
	float NdotL = saturate(dot(input.normal, -lightNorm));

	float3 lightNorm2 = normalize(light1.Direction);
	float NdotL2 = saturate(dot(input.normal, -lightNorm2));

	float4 DirLights = (surfaceColor * (light0.DiffuseColor * NdotL))
		+ (surfaceColor * (light1.DiffuseColor * NdotL2));

	//---------------------------------------------------------------------------------------------
	//Point Light(s)
	float3 dirToPointLight = normalize(lightP0.Position - input.worldPos);
	float pointLightAmount = dot(input.normal, dirToPointLight);

	float4 PLights = (surfaceColor * (pointLightAmount * lightP0.DiffuseColor));

	//---------------------------------------------------------------------------------------------
	//Spot Light(s)
	float3 vLight = normalize(lightS0.Position - input.worldPos);
	float SpotNdotL = max(0.0f, dot(input.normal, vLight));

	//float cosAngle = max(0.0f, dot(-lightS0.Direction, vLight));
	float cosAngle = dot(-lightS0.Direction, vLight);
	float spotAtten = 0;
	
	if (cosAngle <= 1 && cosAngle >= 0.98f)
	{
		//add code for distance later
		spotAtten = smoothstep(0.98f, 1, cosAngle);
	}

	float4 SLights = (surfaceColor*SpotNdotL*lightS0.DiffuseColor)*spotAtten;

	//---------------------------------------------------------------------------------------------


	float4 skyColor = Sky.Sample(basicSampler, reflect(-dirToPointLight, input.normal));
	//---------------------------------------------------------------------------------------------
	
	//return lerp((global + DirLights + PLights + SLights), skyColor, 0.01f);
	//return global + DirLights + PLights + SLights;
	return float4((global.rgb + DirLights.rgb + PLights.rgb + SLights.rgb), surfaceColor.a);
}