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
	float4 cameraPosition;
	DirectionalLight light0;
	DirectionalLight light1;
	PointLight lightP0;
	SpotLight lightS0;
};

static const float RimIntensity = 2.0;

static const float3x3 Gx = {
	-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1 };

static const float3x3 Gy = {
	1, 2, 1,
	0, 0, 0,
	-1,-2,-1 };

Texture2D diffuseTexture : register(t0);
TextureCube Sky			: register(t2);
SamplerState basicSampler : register(s0);

float4 getDirLightColor(DirectionalLight light, VertexToPixel input) {
	float lightAmount = saturate(dot(input.normal, normalize(-light.Direction)));
	return lightAmount * light.DiffuseColor;
}

float4 getPointLightColor(PointLight light, VertexToPixel input) {
	float lightAmount = saturate(dot(input.normal, normalize(light.Position - input.worldPos)));
	return lightAmount * light.DiffuseColor;
}

// Specular highlight for point light
float4 getBlinnSpecular(PointLight light, VertexToPixel input, float4 cameraPosition) {
	float specular = saturate(dot(input.normal, normalize(light.Position - input.worldPos + cameraPosition.xyz - input.worldPos)));
	return pow(specular, 8) * light.DiffuseColor;
}

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

	//Texture
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	//Global Light(s)
	float4 globalLight = ambient0.AmbientColor;

	//Directional Light(s)
	float4 DirLights = getDirLightColor(light0, input)
	+ getDirLightColor(light1, input);

	//Point Light(s)
	float4 PLights = getPointLightColor(lightP0, input);

	//---------------------------------------------------------------------------------------------
	//Spot Light(s)
	float3 vLight = normalize(lightS0.Position - input.worldPos);
	float SpotNdotL = max(0.0f, dot(input.normal, vLight));

	float cosAngle = dot(-lightS0.Direction, vLight);
	float spotAtten = 0;

	if (cosAngle <= 1 && cosAngle >= 0.98f)
	{
		//add code for distance later
		spotAtten = smoothstep(0.98f, 1, cosAngle);
	}

	float4 SLights = (SpotNdotL*lightS0.DiffuseColor)*spotAtten;

	//---------------------------------------------------------------------------------------------

	float3 dirToPointLight = normalize(lightP0.Position - input.worldPos);
	float4 skyColor = Sky.Sample(basicSampler, reflect(-dirToPointLight, input.normal));

	//---------------------------------------------------------------------------------------------

	//// Edge detect
	//float2 PixelOffset = (1.0 / 800.0, 1.0 / 600.0);

	//float Lx = 0;
	//float Ly = 0;

	//for (int y = 0; y <= 2; ++y)
	//{
	//	for (int x = 0; x <= 2; ++x)
	//	{
	//		float2 offset = float2(x-1, y-1)*PixelOffset;

	//		float3 tex = diffuseTexture.Sample(basicSampler, input.uv + offset).rgb;

	//		float luminance = dot(tex, float3(0.3, 0.59, 0.11));

	//		Lx += luminance * Gx[y][x];
	//		Ly += luminance * Gy[y][x];
	//	}
	//}

	//float Line = sqrt((Lx*Lx) + (Ly*Ly));
	//return float4(Line.xxx, 1);

	float3 v = normalize(cameraPosition.xyz - input.worldPos);
	//float vdn = 1.0 - saturate(dot(input.normal, v));
	//return float4(vdn.xxx, 1.0);
	float rim = smoothstep(0.7, 1.0, 1.0 - saturate(dot(input.normal, v)));

	float4 light = 
		globalLight
		+ DirLights
		+ PLights
		+ SLights
		+ getBlinnSpecular(lightP0, input, cameraPosition)
		;

	light = saturate(light) * surfaceColor;

	light = lerp(light, surfaceColor * RimIntensity, rim);
	light = lerp(light, skyColor, 0.01f);

	return light;
}