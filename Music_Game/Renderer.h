//Darren Farr
#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
#include "Entity.h"
#include "Camera.h"
#include "Scene.h"
#include "Lights.h"
#include "GaussianBlur.h"
#include "Bloom.h"
#include "Tex2Dt.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Init(Camera*, ID3D11Device*, ID3D11DeviceContext*, ID3D11RenderTargetView*, IDXGISwapChain*, ID3D11DepthStencilView*, Text2D*, unsigned int width, unsigned int height);
	void Resized(ID3D11DepthStencilView*, ID3D11RenderTargetView*, unsigned int width, unsigned int height);
	void Draw(float, float);

	void SetShaders(SimpleVertexShader*, SimplePixelShader*, SimpleVertexShader*, SimplePixelShader*,
		SimpleVertexShader*, SimplePixelShader*, SimplePixelShader*, SimplePixelShader*, SimpleVertexShader*, SimplePixelShader*);

	void SetScene(Scene*);

	void Setup2D();
	void SetScore(int);
	void Renderer::SetScorePos(DirectX::XMFLOAT2);

private:
	void setWidthHeight(unsigned int width, unsigned int height, ID3D11DepthStencilView* depthStencilView);
	void SetPixelShaderUp(SimplePixelShader*, std::vector<Entity*>, int);
	void CreateAdditionalRSStates();
	void DrawScore();

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShaderNormalMap;
	SimplePixelShader* pixelShaderNormalMap;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;

	SimplePixelShader* pixelShaderBlend;
	SimplePixelShader* pixelShaderNormalMapBlend;

	// Particle Shaders
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;


	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	Camera* Cam;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* backBufferRTV;
	IDXGISwapChain* swapChain;
	ID3D11DepthStencilView* mDepthStencilView;

	unsigned int mWidth, mHeight;
	int score;
	DirectX::XMFLOAT2 scorePos;

	//These may need to be moved
	//-----------------------------------
	ID3D11RenderTargetView* ppRTV;	
	ID3D11ShaderResourceView* ppSRV;
	//-----------------------------------

	GaussianBlur *blur;
	Bloom *bloom;

	Scene* currentScene;

	// Render states
	ID3D11RasterizerState* defaultState;
	ID3D11RasterizerState* rsNoCull;
	ID3D11BlendState* bsAlphaBlend;

	//2D Text
	Text2D* text;
	std::vector<textObject*> flexTextList;
};