#include "Bloom.h"

using namespace DirectX;

const BloomSettings Bloom::DefaultBloomSettings = { 0.50f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f };

Bloom::Bloom(ID3D11Device * device, ID3D11DeviceContext * context, _BloomSettings bloomSettings)
	: mDevice(device), mContext(context), mBloomSettings(bloomSettings), 
	blur(new GaussianBlur(device, context, bloomSettings.BlurAmount))
{
}

Bloom::~Bloom()
{
	delete blur;

	delete mPostProcessVS;
	delete mExtractPS;
	delete mCombinePS;

	mExtractRTV->Release();
	mExtractSRV->Release();
}

void Bloom::Init(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView)
{
	mPostProcessVS = new SimpleVertexShader(mDevice, mContext);
	if (!mPostProcessVS->LoadShaderFile(L"Debug/PostProcessVS.cso"))
		mPostProcessVS->LoadShaderFile(L"PostProcessVS.cso");

	mExtractPS = new SimplePixelShader(mDevice, mContext);
	if (!mExtractPS->LoadShaderFile(L"Debug/BloomExtractPixelShader.cso"))
		mExtractPS->LoadShaderFile(L"BloomExtractPixelShader.cso");

	mCombinePS = new SimplePixelShader(mDevice, mContext);
	if (!mCombinePS->LoadShaderFile(L"Debug/BloomCombinePixelShader.cso"))
		mCombinePS->LoadShaderFile(L"BloomCombinePixelShader.cso");

	blur->Init(width, height, depthStencilView);

	mExtractRTV = 0;
	mExtractSRV = 0;

	setWidthHeight(width, height, depthStencilView);
}

void Bloom::Resize(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView)
{
	setWidthHeight(width, height, depthStencilView);
	blur->Resize(width, height, depthStencilView);
}

void Bloom::setWidthHeight(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView)
{
	mWidth = width;
	mHeight = height;
	mDepthStencilView = depthStencilView;

	if (mExtractRTV) { mExtractRTV->Release(); }
	if (mExtractSRV) { mExtractSRV->Release(); }

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

	ID3D11Texture2D* mExtractTexture;
	mDevice->CreateTexture2D(&textureDesc, 0, &mExtractTexture);

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	mDevice->CreateRenderTargetView(mExtractTexture, &rtvDesc, &mExtractRTV);
	//mDevice->CreateRenderTargetView(mVerticalBlurTexture, &rtvDesc, &mVerticalBlurRTV);

	// Create the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	mDevice->CreateShaderResourceView(mExtractTexture, &srvDesc, &mExtractSRV);

	// We don't need the texture reference itself no mo'
	mExtractTexture->Release();
}

void Bloom::Draw(const float & gameTime, ID3D11ShaderResourceView * inputSRV, ID3D11RenderTargetView * outputRTV)
{
	// Background color for clearing
	const float color[4] = { 0,0,0,0 };

	UINT stride = 0;
	UINT offset = 0;

	mContext->ClearRenderTargetView(mExtractRTV, color);
	mContext->ClearDepthStencilView(mDepthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mContext->OMSetRenderTargets(1, &mExtractRTV, mDepthStencilView);

	// Get the shaders ready for post processing
	mPostProcessVS->SetShader();

	mExtractPS->SetShaderResourceView("InputTexture", inputSRV);
	mExtractPS->SetData("BloomThreshold", &mBloomSettings.BloomThreshold, sizeof(float));
	mExtractPS->CopyAllBufferData();
	mExtractPS->SetShader();

	ID3D11Buffer* nothing = 0;
	mContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	mContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Actually draw exactly 3 vertices
	mContext->Draw(3, 0);

	// Unbind the post process texture from input
	mExtractPS->SetShaderResourceView("InputTexture", 0);

	blur->Draw(gameTime, mExtractSRV, mExtractRTV);

	// Combined
	mContext->ClearRenderTargetView(outputRTV, color);
	mContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mContext->OMSetRenderTargets(1, &outputRTV, mDepthStencilView);

	// Get the shaders ready for post processing
	mPostProcessVS->SetShader();

	mCombinePS->SetShaderResourceView("SceneTexture", inputSRV);
	mCombinePS->SetShaderResourceView("BloomTexture", mExtractSRV);
	mCombinePS->SetData("BloomIntensity", &mBloomSettings.BloomIntensity, sizeof(float));
	mCombinePS->SetData("BloomSaturation", &mBloomSettings.BloomSaturation, sizeof(float));
	mCombinePS->SetData("SceneIntensity", &mBloomSettings.SceneIntensity, sizeof(float));
	mCombinePS->SetData("SceneSaturation", &mBloomSettings.SceneSaturation, sizeof(float));

	mCombinePS->CopyAllBufferData();
	mCombinePS->SetShader();

	mContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	mContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Actually draw exactly 3 vertices
	mContext->Draw(3, 0);

	// Unbind the post process texture from input
	mCombinePS->SetShaderResourceView("SceneTexture", 0);
	mCombinePS->SetShaderResourceView("BloomTexture", 0);
}
