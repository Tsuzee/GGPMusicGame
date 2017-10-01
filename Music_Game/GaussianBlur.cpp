#include "GaussianBlur.h"
#include <iostream>

using namespace DirectX;

const float GaussianBlur::DefaultBlurAmount = 1.0f;
const unsigned int sampleCount = 9;

GaussianBlur::GaussianBlur(ID3D11Device* device, ID3D11DeviceContext * context, float blurAmount)
	: mDevice(device), mContext(context), mBlurAmount(blurAmount)
{
	
}

GaussianBlur::~GaussianBlur()
{
	delete mPostProcessVS;
	delete mGaussianBlurPS;

	mHorizontalBlurRTV->Release();
	mHorizontalBlurSRV->Release();
}

float GaussianBlur::BlurAmount() const
{
	return mBlurAmount;
}

void GaussianBlur::SetBlurAmount(float blurAmount)
{
	mBlurAmount = blurAmount;
}

void GaussianBlur::Init(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView)
{
	mPostProcessVS = new SimpleVertexShader(mDevice, mContext);
	if (!mPostProcessVS->LoadShaderFile(L"Debug/PostProcessVS.cso"))
		mPostProcessVS->LoadShaderFile(L"PostProcessVS.cso");

	mGaussianBlurPS = new SimplePixelShader(mDevice, mContext);
	if (!mGaussianBlurPS->LoadShaderFile(L"Debug/GaussianBlurPixelShader.cso"))
		mGaussianBlurPS->LoadShaderFile(L"GaussianBlurPixelShader.cso");

	mHorizontalBlurRTV = 0;
	mHorizontalBlurSRV = 0;
	
	setWidthHeight(width, height, depthStencilView);
}

void GaussianBlur::Resize(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView)
{
	setWidthHeight(width, height, depthStencilView);
}

void GaussianBlur::setWidthHeight(unsigned int width, unsigned int height, ID3D11DepthStencilView * depthStencilView)
{
	mWidth = width;
	mHeight = height;
	mDepthStencilView = depthStencilView;

	InitializeSampleOffsets();
	InitializeSampleWeights();

	if (mHorizontalBlurRTV) { mHorizontalBlurRTV->Release(); }
	if (mHorizontalBlurSRV) { mHorizontalBlurSRV->Release(); }

	// Create post process resources -----------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* mHorizontalBlurTexture;
	mDevice->CreateTexture2D(&textureDesc, 0, &mHorizontalBlurTexture);
	//ID3D11Texture2D* mVerticalBlurTexture;
	//mDevice->CreateTexture2D(&textureDesc, 0, &mVerticalBlurTexture);

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	mDevice->CreateRenderTargetView(mHorizontalBlurTexture, &rtvDesc, &mHorizontalBlurRTV);
	//mDevice->CreateRenderTargetView(mVerticalBlurTexture, &rtvDesc, &mVerticalBlurRTV);

	// Create the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	mDevice->CreateShaderResourceView(mHorizontalBlurTexture, &srvDesc, &mHorizontalBlurSRV);
	//mDevice->CreateShaderResourceView(mVerticalBlurTexture, &srvDesc, &mVerticalBlurSRV);

	// We don't need the texture reference itself no mo'
	mHorizontalBlurTexture->Release();
	//mVerticalBlurTexture->Release();
}

void GaussianBlur::InitializeSampleOffsets()
{
	float horizontalPixelSize = 1.0f / mWidth;
	float verticalPixelSize = 1.0f / mHeight;

	mHorizontalSampleOffsets.resize(sampleCount);
	mVerticalSampleOffsets.resize(sampleCount);

	mHorizontalSampleOffsets[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	mVerticalSampleOffsets[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	for (UINT i = 0; i < sampleCount / 2; i++)
	{
		float sampleOffset = i * 2 + 1.5f;
		float horizontalOffset = horizontalPixelSize * sampleOffset;
		float verticalOffset = verticalPixelSize * sampleOffset;

		mHorizontalSampleOffsets[i * 2 + 1] = XMFLOAT4(horizontalOffset, 0.0f, 0.0f, 0.0f);
		mHorizontalSampleOffsets[i * 2 + 2] = XMFLOAT4(-horizontalOffset, 0.0f, 0.0f, 0.0f);

		mVerticalSampleOffsets[i * 2 + 1] = XMFLOAT4(0.0f, verticalOffset, 0.0f, 0.0f);
		mVerticalSampleOffsets[i * 2 + 2] = XMFLOAT4(0.0f, -verticalOffset, 0.0f, 0.0f);
	}
}

void GaussianBlur::InitializeSampleWeights()
{
	mSampleWeights.resize(sampleCount);

	mSampleWeights[0] = XMFLOAT4(GetWeight(0), 0.0f, 0.0f, 0.0f);

	float totalWeight = mSampleWeights[0].x;
	for (UINT i = 0; i < sampleCount / 2; i++)
	{
		float weight = GetWeight((float)i + 1);
		mSampleWeights[i * 2 + 1] = XMFLOAT4(weight, 0.0f, 0.0f, 0.0f);
		mSampleWeights[i * 2 + 2] = XMFLOAT4(weight, 0.0f, 0.0f, 0.0f);
		totalWeight += weight * 2;
	}

	// Normalize the weights so that they sum to one
	for (UINT i = 0; i < mSampleWeights.size(); i++)
	{
		//mSampleWeights[i] /= totalWeight;
		mSampleWeights[i] = XMFLOAT4(mSampleWeights[i].x / totalWeight, 0.0f, 0.0f, 0.0f);
	}
}

float GaussianBlur::GetWeight(float x) const
{
	return (float)(exp(-(x * x) / (2 * mBlurAmount * mBlurAmount)));
}

void GaussianBlur::Draw(const float & gameTime, ID3D11ShaderResourceView * inputSRV, ID3D11RenderTargetView * outputRTV)
{
	// Background color for clearing
	const float color[4] = { 0,0,0,0 };

	UINT stride = 0;
	UINT offset = 0;

	mContext->ClearRenderTargetView(mHorizontalBlurRTV, color);
	mContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mContext->OMSetRenderTargets(1, &mHorizontalBlurRTV, mDepthStencilView);

	// Get the shaders ready for post processing
	mPostProcessVS->SetShader();

	mGaussianBlurPS->SetShaderResourceView("InputTexture", inputSRV);
	mGaussianBlurPS->SetData("SampleOffsets", 
		&mHorizontalSampleOffsets[0], sizeof(XMFLOAT4) * sampleCount);
	mGaussianBlurPS->SetData("SampleWeights",
		&mSampleWeights[0], sizeof(XMFLOAT4) * sampleCount);
	mGaussianBlurPS->CopyAllBufferData();
	mGaussianBlurPS->SetShader();

	ID3D11Buffer* nothing = 0;
	mContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	mContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	 //Actually draw exactly 3 vertices
	mContext->Draw(3, 0);

	// Unbind the post process texture from input
	mGaussianBlurPS->SetShaderResourceView("InputTexture", 0);

	// Vertical
	mContext->ClearRenderTargetView(outputRTV, color);
	mContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mContext->OMSetRenderTargets(1, &outputRTV, mDepthStencilView);

	// Get the shaders ready for post processing
	mPostProcessVS->SetShader();

	mGaussianBlurPS->SetShaderResourceView("InputTexture", mHorizontalBlurSRV);

	mGaussianBlurPS->SetData("SampleOffsets",
		&mVerticalSampleOffsets[0], sizeof(XMFLOAT4) * sampleCount);
	mGaussianBlurPS->SetData("SampleWeights",
		&mSampleWeights[0], sizeof(XMFLOAT4) * sampleCount);
	mGaussianBlurPS->CopyAllBufferData();
	mGaussianBlurPS->SetShader();

	mContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	mContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Actually draw exactly 3 vertices
	mContext->Draw(3, 0);

	// Unbind the post process texture from input
	mGaussianBlurPS->SetShaderResourceView("InputTexture", 0);
}