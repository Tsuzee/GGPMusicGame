//Darren Farr
#include "Renderer.h"

using namespace DirectX;

//---------------------------------------------------------
//Default Constructor
//---------------------------------------------------------
Renderer::Renderer()
{
	// Initialize fields
	vertexBuffer = 0;
	indexBuffer = 0;
	vertexShader = 0;
	pixelShader = 0;

	ppRTV = 0;
	ppSRV = 0;
}

//---------------------------------------------------------
//Default Deconstructor
//---------------------------------------------------------
Renderer::~Renderer() 
{
	//May be moved later
	//-----------------------------
	ppSRV->Release();
	ppRTV->Release();
	bsAlphaBlend->Release();
	rsNoCull->Release();

	delete blur;
	delete bloom;
	//-----------------------------
}

//---------------------------------------------------------
//Inititialize one time members
//---------------------------------------------------------
void Renderer::Init(Camera* _Cam, ID3D11Device* device, ID3D11DeviceContext* con, ID3D11RenderTargetView* tView, 
	IDXGISwapChain* chain, ID3D11DepthStencilView* depthStencilView, Text2D* _text, unsigned int width, unsigned int height)
{
	Cam = _Cam;
	mDevice = device;
	context = con;
	backBufferRTV = tView;
	swapChain = chain;
	context->RSGetState(&defaultState);
	text = _text;

	setWidthHeight(width, height, depthStencilView);

	blur = new GaussianBlur(mDevice, context);
	blur->Init(mWidth, mHeight, depthStencilView);

	bloom = new Bloom(mDevice, context);
	bloom->Init(mWidth, mHeight, depthStencilView);

	CreateAdditionalRSStates();
}

//---------------------------------------------------------
//Set Shaders
//---------------------------------------------------------
void Renderer::SetShaders(SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader,
	SimpleVertexShader* _vertShaderNorm, SimplePixelShader* _pixShaderNorm, SimpleVertexShader* _skyVs,
	SimplePixelShader* _skyPs, SimplePixelShader* _pixelShaderB, SimplePixelShader* _pixShaderNormB,
	SimpleVertexShader* _particleVs, SimplePixelShader* _particlePs)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	vertexShaderNormalMap = _vertShaderNorm;
	pixelShaderNormalMap = _pixShaderNorm;
	skyVS = _skyVs;
	skyPS = _skyPs;
	pixelShaderBlend = _pixelShaderB;
	pixelShaderNormalMapBlend = _pixShaderNormB;
	particleVS = _particleVs;
	particlePS = _particlePs;
}

//---------------------------------------------------------
//Set Entities
//---------------------------------------------------------
void Renderer::SetScene(Scene* _scene)
{
	currentScene = _scene;
}

//---------------------------------------------------------
//Draw
//Intitial code from original game.cpp - Chris Cascioli
//Rewritten to work with a list of entities
//---------------------------------------------------------
void Renderer::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	//context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Change render state back to defualt
	context->RSSetState(defaultState);

	// Turn off our custom blend state
	context->OMSetBlendState(nullptr, 0, 0xFFFFFFFF);


	// Post process initial setup =================
	// Start rendering somewhere else!
	context->ClearRenderTargetView(ppRTV, color);
	context->OMSetRenderTargets(1, &ppRTV, mDepthStencilView);

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	//Loop through the list of Entities and draw each one
	//Draw opaque entities first then opaque with normals then transparent with normals and finally transparent objects
	if (currentScene->opaque.size() > 0)
	{
		for (unsigned i = 0; i < currentScene->opaque.size(); i++)
		{
			currentScene->opaque.at(i)->GetMat()->PrepareMaterial(currentScene->opaque.at(i)->GetWorldMat(), Cam->GetViewMatrix(), Cam->GetProjectionMatrix(), vertexShader);
			SetPixelShaderUp(pixelShader, currentScene->opaque, i);

			stride = sizeof(Vertex);
			offset = 0;

			vertexBuffer = currentScene->opaque.at(i)->GetMesh()->GetVertexBuffer(); //Store the vertex buffer address
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(currentScene->opaque.at(i)->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			// Finally do the actual drawing
			//  - Do this ONCE PER OBJECT you intend to draw
			//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
			//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
			//     vertices in the currently set VERTEX BUFFER
			context->DrawIndexed(
				currentScene->opaque.at(i)->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // Offset to add to each index when looking up vertices
		}
	}//end of opaque draw calls

	if (currentScene->opaqueNorm.size() > 0)
	{
		for (unsigned i = 0; i < currentScene->opaqueNorm.size(); i++)
		{
			currentScene->opaqueNorm.at(i)->GetMat()->PrepareMaterial(currentScene->opaqueNorm.at(i)->GetWorldMat(), Cam->GetViewMatrix(), Cam->GetProjectionMatrix(), vertexShaderNormalMap);
			SetPixelShaderUp(pixelShaderNormalMap, currentScene->opaqueNorm, i);

			stride = sizeof(Vertex);
			offset = 0;

			vertexBuffer = currentScene->opaqueNorm.at(i)->GetMesh()->GetVertexBuffer(); //Store the vertex buffer address
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(currentScene->opaqueNorm.at(i)->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(
				currentScene->opaqueNorm.at(i)->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // Offset to add to each index when looking up vertices
		}
	}//end of opaque with normal maps draw calls

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//Draw the skybox if one is loaded
	if (currentScene->background != NULL)
	{
		vertexBuffer = currentScene->background->GetMesh()->GetVertexBuffer();
		indexBuffer = currentScene->background->GetMesh()->GetIndexBuffer();

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		currentScene->background->GetMat()->PrepareSkybox(Cam->GetViewMatrix(), Cam->GetProjectionMatrix(), skyVS, skyPS);

		context->RSSetState(currentScene->background->GetMat()->GetRast());
		context->OMSetDepthStencilState(currentScene->background->GetMat()->GetDepthSD(), 0);
		context->DrawIndexed(currentScene->background->GetMesh()->GetIndexCount(), 0, 0);

		// Reset the render states we've changed
		context->RSSetState(0);
		context->OMSetDepthStencilState(0, 0);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//Draw particles if there are any
	if (currentScene->Particles != nullptr)
	{
		// Particle states
		float blend[4] = { 1,1,1,1 };
		context->OMSetBlendState(currentScene->Particles->GetMaterial()->GetParticleBlendState(), blend, 0xffffffff);  // Additive blending
		context->OMSetDepthStencilState(currentScene->Particles->GetMaterial()->GetParticleDepthState(), 0);			// No depth WRITING

		currentScene->Particles->SetShaders(particleVS, particlePS);															// Draw the emitter
		currentScene->Particles->Draw(context, Cam);

		// Reset to default states for next frame
		context->RSSetState(0);
		context->OMSetDepthStencilState(0, 0);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//Change render states based on blending/transperancy
	//Turn off back face culling
	context->RSSetState(rsNoCull);

	// Turn on our custom blend state to enable alpha blending
	context->OMSetBlendState(
		bsAlphaBlend,
		0, // Not using per-channel blend factors
		0xFFFFFFFF); // Sample mask - Need all bits set (0xFFFFFFFF)

	if (currentScene->transparentNorm.size() > 0)
	{
		for (unsigned i = 0; i < currentScene->transparentNorm.size(); i++)
		{
			currentScene->transparentNorm.at(i)->GetMat()->PrepareMaterial(currentScene->transparentNorm.at(i)->GetWorldMat(), Cam->GetViewMatrix(), Cam->GetProjectionMatrix(), vertexShaderNormalMap);
			SetPixelShaderUp(pixelShaderNormalMapBlend, currentScene->transparentNorm, i);

			stride = sizeof(Vertex);
			offset = 0;

			vertexBuffer = currentScene->transparentNorm.at(i)->GetMesh()->GetVertexBuffer(); //Store the vertex buffer address
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(currentScene->transparentNorm.at(i)->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(
				currentScene->transparentNorm.at(i)->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // Offset to add to each index when looking up vertices
		}
	}//end of transparent with normal maps draw calls

	if (currentScene->transparent.size() > 0)
	{
		for (unsigned i = 0; i < currentScene->transparent.size(); i++)
		{
			currentScene->transparent.at(i)->GetMat()->PrepareMaterial(currentScene->transparent.at(i)->GetWorldMat(), Cam->GetViewMatrix(), Cam->GetProjectionMatrix(), vertexShader);
			SetPixelShaderUp(pixelShaderBlend, currentScene->transparent, i);

			stride = sizeof(Vertex);
			offset = 0;

			vertexBuffer = currentScene->transparent.at(i)->GetMesh()->GetVertexBuffer(); //Store the vertex buffer address
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(currentScene->transparent.at(i)->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(
				currentScene->transparent.at(i)->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // Offset to add to each index when looking up vertices
		}
		
	}//end of transparent with normal maps draw calls

	//Draw Text Here
	text->DrawMyText();

	if (currentScene->name == "MainGame" || currentScene->name == "Results")
	{
		DrawScore();
	}

	// Reset states to properly render next frame
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	float factors[4] = { 1,1,1,1 };
	context->OMSetBlendState(0, factors, 0xFFFFFFFF);

	//blur->Draw(deltaTime, ppSRV, backBufferRTV);
	bloom->Draw(deltaTime, ppSRV, backBufferRTV);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

//---------------------------------------------------------
//Set the render width and height
//---------------------------------------------------------
void Renderer::setWidthHeight(unsigned int width, unsigned int height, ID3D11DepthStencilView * depthStencilView)
{
	mWidth = width;
	mHeight = height;
	mDepthStencilView = depthStencilView;

	if (ppRTV) { ppRTV->Release(); }
	if (ppSRV) { ppSRV->Release(); }

	// Create post process resources -----------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = mWidth;
	textureDesc.Height = mHeight;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* ppTexture;
	mDevice->CreateTexture2D(&textureDesc, 0, &ppTexture);

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	mDevice->CreateRenderTargetView(ppTexture, &rtvDesc, &ppRTV);

	// Create the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	mDevice->CreateShaderResourceView(ppTexture, &srvDesc, &ppSRV);

	// We don't need the texture reference itself no mo'
	ppTexture->Release();
}

//---------------------------------------------------------
//Set the pixel shader up
//---------------------------------------------------------
void Renderer::SetPixelShaderUp(SimplePixelShader* pShader, std::vector<Entity*> list, int i)
{
	pShader->SetFloat4("cameraPosition", Cam->GetPositon());

	if (currentScene->spotLights.size() > 0) {
		currentScene->spotLights.at(0)->Direction.x = Cam->GetDirection().x;
		currentScene->spotLights.at(0)->Direction.y = Cam->GetDirection().y;
		currentScene->spotLights.at(0)->Direction.z = Cam->GetDirection().z;
		currentScene->spotLights.at(0)->Position.x = Cam->GetPositon().x;
		currentScene->spotLights.at(0)->Position.y = Cam->GetPositon().y;
		currentScene->spotLights.at(0)->Position.z = Cam->GetPositon().z;
	}

	//loop through lights
	//global lights
	for (unsigned int g = 0; g < currentScene->globalLights.size(); g++)
	{
		std::string name = "ambient" + std::to_string(g);
		//setup lights
		pShader->SetData(
			name,
			currentScene->globalLights.at(g),
			sizeof(GlobalLight)
		);
	}

	//directional lights
	for (unsigned int d = 0; d < currentScene->directionalLights.size(); d++)
	{
		std::string name = "light" + std::to_string(d);
		//setup lights
		pShader->SetData(
			name,
			currentScene->directionalLights.at(d),
			sizeof(DirectionalLight)
		);
	}

	//point lights
	for (unsigned int p = 0; p < currentScene->pointLights.size(); p++)
	{
		std::string name = "lightP" + std::to_string(p);
		//setup lights
		pShader->SetData(
			name,
			currentScene->pointLights.at(p),
			sizeof(PointLight)
		);
	}

	//spot lights
	for (unsigned int s = 0; s < currentScene->spotLights.size(); s++)
	{
		std::string name = "lightS" + std::to_string(s);
		//setup lights
		pShader->SetData(
			name,
			currentScene->spotLights.at(s),
			sizeof(SpotLight)
		);
	}

	pShader->SetSamplerState("basicSampler", list.at(i)->GetMat()->GetSampleState());
	pShader->SetShaderResourceView("diffuseTexture", list.at(i)->GetMat()->GetSRV());

	//check for a normal map
	if (list.at(i)->GetMat()->HasNormalMap())
	{
		pShader->SetShaderResourceView("NormalMap", list.at(i)->GetMat()->GetNormalSRV());
	}

	if (currentScene->background != NULL)
	{
		pShader->SetShaderResourceView("Sky", currentScene->background->GetMat()->GetSkySRV());
	}

	pShader->CopyAllBufferData();

	pShader->SetShader();
}

//---------------------------------------------------------
//Create additional render states
//---------------------------------------------------------
void Renderer::CreateAdditionalRSStates()
{
	// Set up a rasterizer state with no culling
	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_NONE;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.DepthClipEnable = true;
	mDevice->CreateRasterizerState(&rd, &rsNoCull);

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	mDevice->CreateBlendState(&bd, &bsAlphaBlend);
}


//---------------------------------------------------------
//Update the depth Stencil View
//Program Errors on window resize if this is not updated
//---------------------------------------------------------
void Renderer::Resized(ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* bBRTV, unsigned int width, unsigned int height)
{
	backBufferRTV = bBRTV;

	setWidthHeight(width, height, depthStencilView);

	blur->Resize(width, height, depthStencilView);
	bloom->Resize(width, height, depthStencilView);
}

//---------------------------------------------------------
//Draw player score
//---------------------------------------------------------
void Renderer::DrawScore()
{
	text->DrawLiveText(score, scorePos);
}

//---------------------------------------------------------
//Set player score
//---------------------------------------------------------
void Renderer::SetScore(int _score)
{
	score = _score;
}

//---------------------------------------------------------
//Set Score screen position
//---------------------------------------------------------
void Renderer::SetScorePos(DirectX::XMFLOAT2 _pos)
{
	scorePos = _pos;
}

//---------------------------------------------------------
//Sets up for 2d text drawing
//---------------------------------------------------------
void Renderer::Setup2D()
{
	
}