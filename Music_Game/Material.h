//Darren Farr
#pragma once

#include <DirectXMath.h>
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"


class Material
{
public:
	Material();
	Material(ID3D11Device*, ID3D11DeviceContext*, const wchar_t*);
	Material::Material(ID3D11Device*, ID3D11DeviceContext*, const wchar_t*, bool);
	Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path, SimpleVertexShader* particleVS, SimplePixelShader* particlePS);
	~Material();

	void PrepareMaterial(DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, SimpleVertexShader*);
	void PrepareSkybox(DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, SimpleVertexShader*, SimplePixelShader*);

	void SetTexture(ID3D11Device*, ID3D11DeviceContext*, const wchar_t*);
	void SetupSkybox(ID3D11Device*, ID3D11DeviceContext*, const wchar_t*);
	void SetupParticle(ID3D11Device*, ID3D11DeviceContext*, const wchar_t*, SimpleVertexShader*, SimplePixelShader*);
	ID3D11RasterizerState* GetRast();
	ID3D11DepthStencilState* GetDepthSD();

	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView* GetSkySRV();
	ID3D11SamplerState* GetSampleState();

private:
	
	ID3D11ShaderResourceView* SRV;
	ID3D11SamplerState* sampleState;
	D3D11_SAMPLER_DESC* sampleDes;

	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* rsSky;
	ID3D11DepthStencilState* dsSky;

	// Particle Texture
	ID3D11ShaderResourceView* particleTexture;

	// Particle Resources
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;
};