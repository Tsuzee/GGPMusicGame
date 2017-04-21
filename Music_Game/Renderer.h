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

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Init(Camera*, ID3D11DeviceContext*, ID3D11RenderTargetView*, IDXGISwapChain*, ID3D11DepthStencilView*, ID3D11Device*);
	void Resized(ID3D11DepthStencilView*, ID3D11RenderTargetView*);
	void Draw(float, float);
	void SetShaders(SimpleVertexShader*, SimplePixelShader*, SimpleVertexShader*, SimplePixelShader*, 
		SimpleVertexShader*, SimplePixelShader*, SimplePixelShader*, SimplePixelShader*);
	void SetScene(Scene*);

private:
	void SetPixelShaderUp(SimplePixelShader*, int);
	void CreateAdditionalRSStates();

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShaderNormalMap;
	SimplePixelShader* pixelShaderNormalMap;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;
	SimplePixelShader* pixelShaderBlend;
	SimplePixelShader* pixelShaderNormalMapBlend;

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11Device* device;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* backBufferRTV;
	IDXGISwapChain* swapChain;
	ID3D11DepthStencilView* depthStencilView;

	Scene* currentScene;

	Camera* Cam;

	// Render states
	ID3D11RasterizerState* defaultState;
	ID3D11RasterizerState* rsNoCull;
	ID3D11BlendState* bsAlphaBlend;
};