//Original code - Chris Cascioli
//Modified for use in homework

#include "Game.h"
#include <time.h> 
#include <iostream>
// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;
	vertexShaderNormalMap = 0;
	pixelShaderNormalMap = 0;

	MusicPlayer musicPlayer();

	SceneBuilder SceneBuild();

	SceneManager SceneManag();
	Renderer Render();
	Camera Cam(width, height);

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	//if (vertexBuffer) { vertexBuffer->Release(); }

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
	delete vertexShaderNormalMap;
	delete pixelShaderNormalMap;
	delete skyVS;
	delete skyPS;
	delete pixelShaderBlend;
	delete pixelShaderNormalMapBlend;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	Cam.SetWidthHeight(width, height);
	Cam.Init();

	LoadShaders();

	Render.SetShaders(vertexShader, pixelShader, vertexShaderNormalMap, pixelShaderNormalMap, skyVS, skyPS, pixelShaderBlend, pixelShaderNormalMapBlend);

	SceneBuild.Init(device, context);
	SceneManag.AddScene(SceneBuild.GetScene(1));
	SceneManag.AddScene(SceneBuild.GetScene(2));
	SceneManag.AddScene(SceneBuild.GetScene(3));

	//Start with scene 1
	SceneNumber = 1;
	setScene();

	Render.Init(&Cam, context, backBufferRTV, swapChain, depthStencilView, device);

	if (SceneManag.GetScene(SceneNumber)->musicFileName) {
		musicPlayer.setSound(SceneManag.GetScene(SceneNumber)->musicFileName);
		musicPlayer.play();
	}

	player = Player(SceneBuild.GetPlayerEntity());

	//Make 5 Asteroids for the game
	asteroid = Asteroid(SceneBuild.GetAsteroidEntity(0));
	asteroid2 = Asteroid(SceneBuild.GetAsteroidEntity(1));
	asteroid3 = Asteroid(SceneBuild.GetAsteroidEntity(2));
	asteroid4 = Asteroid(SceneBuild.GetAsteroidEntity(3));
	asteroid5 = Asteroid(SceneBuild.GetAsteroidEntity(4));



	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	vertexShaderNormalMap = new SimpleVertexShader(device, context);
	if (!vertexShaderNormalMap->LoadShaderFile(L"Debug/VertexShaderNormalMap.cso"))
		vertexShaderNormalMap->LoadShaderFile(L"VertexShaderNormalMap.cso");

	pixelShaderNormalMap = new SimplePixelShader(device, context);
	if (!pixelShaderNormalMap->LoadShaderFile(L"Debug/PixelShaderNormalMap.cso"))
		pixelShaderNormalMap->LoadShaderFile(L"PixelShaderNormalMap.cso");

	skyVS = new SimpleVertexShader(device, context);
	if (!skyVS->LoadShaderFile(L"Debug/SkyVS.cso"))
		skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	if (!skyPS->LoadShaderFile(L"Debug/SkyPS.cso"))
		skyPS->LoadShaderFile(L"SkyPS.cso");

	pixelShaderBlend = new SimplePixelShader(device, context);
	if (!pixelShaderBlend->LoadShaderFile(L"Debug/BlendPixelShader.cso"))
		pixelShaderBlend->LoadShaderFile(L"BlendPixelShader.cso");

	pixelShaderNormalMapBlend = new SimplePixelShader(device, context);
	if (!pixelShaderNormalMapBlend->LoadShaderFile(L"Debug/PixelShaderNormalMapBlend.cso"))
		pixelShaderNormalMapBlend->LoadShaderFile(L"PixelShaderNormalMapBlend.cso");
}

void Game::setScene()
{
	//Tell the game which scene it should be rendering, uses 1 based indexing
	Render.SetScene(SceneManag.GetScene(SceneNumber));

	if (SceneManag.GetScene(SceneNumber)->musicFileName) {
		musicPlayer.setSound(SceneManag.GetScene(SceneNumber)->musicFileName);
		musicPlayer.play();
	}
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	Cam.Resize(width, height);

	DXCore::OnResize();
	Render.Resized(depthStencilView, backBufferRTV);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	//If in scene 1 go to scene 2
	if (GetAsyncKeyState(VK_RETURN) && SceneNumber == 1)
	{
		//SceneNumber = 2;
		//setScene();
	}

	musicPlayer.update();

	Cam.Update(prevMousePos, deltaTime);

	Scene *currentScene = SceneManag.GetScene(SceneNumber);

	//Temp code
	//----------------------------------------------------------------------------------------
	
	//Spawn an inactive asteroid in a random lane.
	srand(time(NULL));
	if (!asteroid.IsActive()) { asteroid.SetActive(rand() % 3 + 1); }
	else if (!asteroid2.IsActive()) { asteroid2.SetActive(rand() % 2 + 1); }
	else if (!asteroid3.IsActive()) { asteroid3.SetActive(rand() % 3 + 1); }
	else if (!asteroid4.IsActive()) { asteroid4.SetActive(rand() % 2 + 1); }
	else if (!asteroid5.IsActive()) { asteroid5.SetActive(rand() % 3 + 1); }


	player.Update(deltaTime);
	asteroid.Update(deltaTime);
	asteroid2.Update(deltaTime);
	asteroid3.Update(deltaTime);
	asteroid4.Update(deltaTime);
	asteroid5.Update(deltaTime);
	for each (Entity* ent in currentScene->entities)
	{
		ent->Update();
	}

	XMMATRIX playerWorld = XMLoadFloat4x4(&(SceneBuild.GetPlayerEntity()->GetWorldMat()));
	XMMATRIX playerWorldSpace = XMLoadFloat4x4(&(SceneBuild.GetPlayerEntity()->GetWorldMat()));

	XMMATRIX asteroidWorld = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(0)->GetWorldMat()));
	XMMATRIX asteroidWorld2 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(1)->GetWorldMat()));
	XMMATRIX asteroidWorld3 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(2)->GetWorldMat()));
	XMMATRIX asteroidWorld4 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(3)->GetWorldMat()));
	XMMATRIX asteroidWorld5 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(4)->GetWorldMat()));
	XMMATRIX asteroidWorldSpace = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(0)->GetWorldMat()));
	XMMATRIX asteroidWorldSpace2 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(1)->GetWorldMat()));
	XMMATRIX asteroidWorldSpace3 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(2)->GetWorldMat()));
	XMMATRIX asteroidWorldSpace4 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(3)->GetWorldMat()));
	XMMATRIX asteroidWorldSpace5 = XMLoadFloat4x4(&(SceneBuild.GetAsteroidEntity(4)->GetWorldMat()));

	bool collide = Collision::Instance().BoundingSphereCollision(player.GetCollider()->GetBoudingSphere(),
		playerWorldSpace,
		asteroid.GetCollider()->GetBoudingSphere(),
		asteroidWorldSpace);
	if (collide) { printf("Asteroid 1 collided with player\n"); }
	collide = false;

	collide = Collision::Instance().BoundingSphereCollision(player.GetCollider()->GetBoudingSphere(),
		playerWorldSpace,
		asteroid2.GetCollider()->GetBoudingSphere(),
		asteroidWorldSpace2);
	if (collide) { printf("Asteroid 2 collided with player\n"); }
	collide = false;

	collide = Collision::Instance().BoundingSphereCollision(player.GetCollider()->GetBoudingSphere(),
		playerWorldSpace,
		asteroid3.GetCollider()->GetBoudingSphere(),
		asteroidWorldSpace3);
	if (collide) { printf("Asteroid 3 collided with player\n"); }
	collide = false;

	collide = Collision::Instance().BoundingSphereCollision(player.GetCollider()->GetBoudingSphere(),
		playerWorldSpace,
		asteroid4.GetCollider()->GetBoudingSphere(),
		asteroidWorldSpace4);
	if (collide) { printf("Asteroid 4 collided with player\n"); }
	collide = false;

	collide = Collision::Instance().BoundingSphereCollision(player.GetCollider()->GetBoudingSphere(),
		playerWorldSpace,
		asteroid5.GetCollider()->GetBoudingSphere(),
		asteroidWorldSpace5);
	if (collide) { printf("Asteroid 5 collided with player\n"); }

}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	Render.Draw(deltaTime, totalTime);
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	Cam.MouseDown(true);

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	Cam.MouseDown(false);

	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion