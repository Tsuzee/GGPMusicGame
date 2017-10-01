#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "SimpleShader.h"

class GaussianBlur
{
public:
	GaussianBlur(ID3D11Device* device, ID3D11DeviceContext* context, float blurAmount = DefaultBlurAmount);
	~GaussianBlur();

	void Init(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);
	void Resize(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);

	float BlurAmount() const;
	void SetBlurAmount(float blurAmount);

	//virtual void Initialize() override;
	void Draw(const float& gameTime, ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);

private:
	//GaussianBlur();
	//GaussianBlur(const GaussianBlur& rhs);
	//GaussianBlur& operator=(const GaussianBlur& rhs);

	void setWidthHeight(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);

	void InitializeSampleOffsets();
	void InitializeSampleWeights();
	float GetWeight(float x) const;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
	ID3D11DepthStencilView* mDepthStencilView;

	float mBlurAmount;
	static const float DefaultBlurAmount;

	unsigned int mWidth, mHeight;

	SimpleVertexShader* mPostProcessVS;
	SimplePixelShader* mGaussianBlurPS;

	ID3D11RenderTargetView* mHorizontalBlurRTV;
	//ID3D11RenderTargetView* mVerticalBlurRTV;

	ID3D11ShaderResourceView* mHorizontalBlurSRV;
	//ID3D11ShaderResourceView *mVerticalBlurSRV;

	std::vector<DirectX::XMFLOAT4> mHorizontalSampleOffsets;
	std::vector<DirectX::XMFLOAT4> mVerticalSampleOffsets;
	std::vector<DirectX::XMFLOAT4> mSampleWeights;
};

