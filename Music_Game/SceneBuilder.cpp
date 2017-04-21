//Darren Farr
#include "SceneBuilder.h"

using namespace DirectX;

//---------------------------------------------------------
//Default Constructor
//---------------------------------------------------------
SceneBuilder::SceneBuilder() {}

//---------------------------------------------------------
//Default Deconstructor
//---------------------------------------------------------
SceneBuilder::~SceneBuilder()
{
	delete asteroidMat;
	delete menuMat;
	//delete creditsMat;
	delete playerMat;
	delete backgroundMat;
	delete laneMat;
	delete plutoMat;

	delete cubeMesh;
	delete asteroidMesh;
	delete quadMesh;
	delete playerMesh;
	delete skyboxMesh;

	delete menuEnt;
	delete playerEnt;
	delete asteroidEnt;
	delete asteroidEnt2;
	delete asteroidEnt3;
	delete asteroidEnt4;
	delete asteroidEnt5;
	delete menuBackgroundEnt;
	delete gameBackgroundEnt;
	//delete creditsBackgroundEnt;
	delete laneEnt;
	delete plutoEnt;
	delete testNormals;

	delete ambient;
	delete dirLight;
	delete dirLight2;
	delete dirLight3;
	delete dirLight4;
	delete pointLight;
	delete spotLight;

	delete scene1;
	delete scene2;
	delete scene3;


}

//---------------------------------------------------------
//Set up a scene
//---------------------------------------------------------
void SceneBuilder::Init(ID3D11Device *_device, ID3D11DeviceContext *_context)
{
	asteroidList[5];
	device = _device;
	context = _context;
	BuildMaterials();
	BuildLights();
	BuildMeshes();
	BuildEntities();
	SetupScenes();
}

//---------------------------------------------------------
//Build Materials Here
//---------------------------------------------------------
void SceneBuilder::BuildMaterials()
{
	//Texture file path
	const wchar_t* path;

	path = L"Assets/textures/menu.png";
	menuMat = new Material(device, context, path);

	path = L"Assets/textures/player.png";
	playerMat = new Material(device, context, path);

	//path = L"Assets/textures/asteroid.png";
	path = L"Assets/textures/rock.jpg";
	asteroidMat = new Material(device, context, path);
	path = L"Assets/textures/rockNormals.jpg";
	asteroidMat->SetNormalMap(device, context, path);

	path = L"Assets/textures/pluto.jpg";
	plutoMat = new Material(device, context, path);

	path = L"Assets/textures/spaceBackground.dds";
	//path = L"Assets/textures/SunnyCubeMap.dds";
	backgroundMat = new Material(device, context, path, true);

	path = L"Assets/textures/rainbow.jpg";
	laneMat = new Material(device, context, path);
}

//---------------------------------------------------------
//Setup lights
//---------------------------------------------------------
void SceneBuilder::BuildLights()
{
	//global ambient light
	ambient = new GlobalLight();
	ambient->AmbientColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1);

	//Directional Lights
	//-----------------------------------------------------------
	dirLight = new DirectionalLight();
	dirLight->DiffuseColor = XMFLOAT4(0, 0, 1, 1);
	dirLight->Direction = XMFLOAT3(1, -1, 0);

	dirLight2 = new DirectionalLight();
	dirLight2->DiffuseColor = XMFLOAT4(1, 0, 0, 1);
	dirLight2->Direction = XMFLOAT3(1, 1, 0);

	dirLight3 = new DirectionalLight();
	dirLight3->DiffuseColor = XMFLOAT4(1, 1, 1, 1);
	dirLight3->Direction = XMFLOAT3(0, 0, 1);

	dirLight4 = new DirectionalLight();
	dirLight4->DiffuseColor = XMFLOAT4(1, 1, 1, 1);
	dirLight4->Direction = XMFLOAT3(0, 0, -1);

	//Point Lights
	//-----------------------------------------------------------
	pointLight = new PointLight();
	pointLight->DiffuseColor = XMFLOAT4(1, 1, 0, 1);
	pointLight->Position = XMFLOAT3(2, 2, -6);

	//Spot Lights
	//-----------------------------------------------------------
	spotLight = new SpotLight();
	spotLight->DiffuseColor = XMFLOAT4(0.85f, 0.85f, 0.85f, 1);
	spotLight->Direction = XMFLOAT3(0, 0, 1);
	spotLight->phi = 0.0f;
	spotLight->Position = XMFLOAT3(0, 0, 0);
	spotLight->theta = 90.0f;
}

//---------------------------------------------------------
//Build Meshes Here
//---------------------------------------------------------
void SceneBuilder::BuildMeshes()
{
	cubeMesh = new Mesh("cube", device, false);

	skyboxMesh = new Mesh("cube", device, true);

	quadMesh = new Mesh("quad", device, false);

	sphereMesh = new Mesh("sphere", device, false);

	playerMesh = new Mesh("sphere", device, false);

	asteroidMesh = new Mesh("sphereTest", device, true);
}

//---------------------------------------------------------
//Build Base Entities Here
//---------------------------------------------------------
void SceneBuilder::BuildEntities()
{
	//Entity template (mesh name, material name, position, rotation, scale)

	menuEnt = new Entity(quadMesh, menuMat, XMFLOAT3(+0.0f, +3.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+1.0f, +1.0f, +1.0f));

	playerEnt = new Entity(playerMesh, playerMat, XMFLOAT3(0.0f, -1.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+1.0f, +1.0f, +1.0f));

	asteroidEnt = new Entity(asteroidMesh, asteroidMat, XMFLOAT3(2.0f, 1.5f, -10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));
	asteroidList[0] = asteroidEnt;
	asteroidEnt2 = new Entity(asteroidMesh, asteroidMat, XMFLOAT3(0.0f, 1.5f, -10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));
	asteroidList[1] = asteroidEnt2;
	asteroidEnt3 = new Entity(asteroidMesh, asteroidMat, XMFLOAT3(-2.0f, 1.5f, -10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));
	asteroidList[2] = asteroidEnt3;
	asteroidEnt4 = new Entity(asteroidMesh, asteroidMat, XMFLOAT3(0.0f, 1.5f, -10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));
	asteroidList[3] = asteroidEnt4;
	asteroidEnt5 = new Entity(asteroidMesh, asteroidMat, XMFLOAT3(2.0f, 1.5f, -10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));
	asteroidList[4] = asteroidEnt5;

	plutoEnt = new Entity(sphereMesh, plutoMat, XMFLOAT3(+8.0f, +7.0f, 10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+3.0f, +3.0f, +3.0f));

	menuBackgroundEnt = new Entity(skyboxMesh, backgroundMat, XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+10.0f, +10.0f, +10.0f));
	gameBackgroundEnt = new Entity(skyboxMesh, backgroundMat, XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+10.0f, +10.0f, +10.0f));
	//creditsBackgroundEnt = new Entity(cubeMesh, backgroundMat, XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+10.0f, +10.0f, +10.0f));

	laneEnt = new Entity(quadMesh, laneMat, XMFLOAT3(-2.0f, -1.0f, 10.0f), XMFLOAT3(+1.5f, +0.0f, +0.0f), XMFLOAT3(+1.0f, +2.0f, +2.0f));

	testNormals = new Entity(asteroidMesh, asteroidMat, XMFLOAT3(0, 6, -8), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+3.0f, +3.0f, +3.0f));
}

//---------------------------------------------------------
//Setup the 3 built in scenes here
//---------------------------------------------------------
void SceneBuilder::SetupScenes()
{
	//will be for scene 1
	scene1 = new Scene();
	scene1->name = "Menu";
	scene1->entities = std::vector<Entity*>();
	scene1->globalLights.push_back(ambient);
	scene1->directionalLights.push_back(dirLight3);
	scene1->entities.push_back(testNormals);
	scene1->directionalLights.push_back(dirLight);
	scene1->directionalLights.push_back(dirLight2);
	scene1->directionalLights.push_back(dirLight4);

	scene1->background = menuBackgroundEnt;
	

	//Scene 2
	//------------------------------------------------------------------
	scene2 = new Scene();
	scene2->name = "MainGame";
	scene2->entities = std::vector<Entity*>();
	scene2->background = gameBackgroundEnt;
	scene2->globalLights.push_back(ambient);
	scene2->entities.push_back(playerEnt);
	scene2->entities.push_back(laneEnt);
	scene2->entities.push_back(asteroidEnt);
	scene2->entities.push_back(asteroidEnt2);
	scene2->entities.push_back(asteroidEnt3);
	scene2->entities.push_back(asteroidEnt4);
	scene2->entities.push_back(asteroidEnt5);
	scene2->entities.push_back(plutoEnt);
	scene2->musicFileName = "04_-_Bloody_Revenge.mp3";


	
	
	//Scene 3
	//------------------------------------------------------------------
	scene3 = new Scene();
	scene3->name = "Game Over";
	scene3->entities = std::vector<Entity*>();
	scene3->directionalLights.push_back(dirLight4);
	//scene3->background = creditsBackgroundEnt;
	scene3->musicFileName = "04_-_Bloody_Revenge.mp3";
}


//---------------------------------------------------------
//Create Entities found in the scene
//---------------------------------------------------------
Entity* SceneBuilder::CreateEntity(Mesh* mesh, Material* mat, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale)
{
	Entity* ent = new Entity(mesh, mat, pos, rot, scale);

	return ent;
}

//---------------------------------------------------------
//Return the list of entities in the scene
//---------------------------------------------------------
Scene* SceneBuilder::GetScene(int num)
{
	if (num == 1) { return scene1; }
	else if (num == 2) { return scene2; }
	else if (num == 3) { return scene3; }
	else { return nullptr; }

	return scene1;
}

//---------------------------------------------------------
//Return the player Entity
//This needs to go, needs to be requested from the scene itself
//---------------------------------------------------------
Entity* SceneBuilder::GetPlayerEntity()
{
	return playerEnt;
}

Entity* SceneBuilder::GetAsteroidEntity(int num)
{
	return asteroidList[num];
}

