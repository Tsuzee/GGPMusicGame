#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleShader.h"
#include "GaussianBlur.h"

typedef struct _BloomSettings
{
	float BloomThreshold;
	float BloomIntensity;
	float BloomSaturation;
	float BlurAmount;
	float SceneIntensity;
	float SceneSaturation;
} BloomSettings;

class Bloom
{
public:
	Bloom(ID3D11Device* device, ID3D11DeviceContext* context, _BloomSettings bloomSettings = DefaultBloomSettings);
	~Bloom();

	void Init(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);
	void Resize(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);
	
	//const BloomSettings& GetBloomSettings() const;
	//void SetBloomSettings(const BloomSettings& bloomSettings);

	void Draw(const float& gameTime, ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);

private:

	void setWidthHeight(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);

	BloomSettings mBloomSettings;

	static const BloomSettings DefaultBloomSettings;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
	ID3D11DepthStencilView* mDepthStencilView;

	float mBlurAmount;
	static const float DefaultBlurAmount;

	unsigned int mWidth, mHeight;

	GaussianBlur *blur;

	SimpleVertexShader* mPostProcessVS;
	SimplePixelShader* mExtractPS;
	SimplePixelShader* mCombinePS;

	ID3D11RenderTargetView* mExtractRTV;
	ID3D11ShaderResourceView* mExtractSRV;
};