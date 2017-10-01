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
	delete creditsMat;
	delete playerMat;
	delete backgroundMat;
	delete sunMat;
	delete earthMat;
	delete moonMat;
	delete venusMat;
	delete laneMat;
	delete p1Mat;
	delete p2Mat;
	delete p3Mat;
	delete lane2Mat;
	delete particelMat;

	delete cubeMesh;
	delete asteroidMesh;
	delete sphereMesh;
	delete quadMesh;
	delete playerMesh;
	delete skyboxMesh;

	delete menuEnt;
	delete playerEnt;

	delete menuBackgroundEnt;
	delete gameBackgroundEnt;
	delete creditsBackgroundEnt;

	delete sunEnt;
	delete earthEnt;
	delete venusEnt;
	delete moonEnt;
	delete laneEnt;
	delete laneEnt2;

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
	delete scene4;


	for (Entity *e : asteroidList) {
		delete e;
	}

	delete p1;
	delete p2;
	delete p3;

	delete emitter;

	delete titleEnt;
	delete titleMat;
}

//---------------------------------------------------------
//Set up a scene
//---------------------------------------------------------
void SceneBuilder::Init(ID3D11Device *_device, ID3D11DeviceContext *_context)
{
	asteroidList[12];
	device = _device;
	context = _context;
	BuildMaterials();
	BuildLights();
	BuildMeshes();
	BuildEntities();
	BuildParticles();
	SetupScenes();
}

//---------------------------------------------------------
//Build Materials Here
//---------------------------------------------------------
void SceneBuilder::BuildMaterials()
{
	//Texture file path
	const wchar_t* path;

	path = L"Assets/textures/player.png";
	playerMat = new Material(device, context, path);

	path = L"Assets/textures/rock.jpg";
	asteroidMat = new Material(device, context, path);
	path = L"Assets/textures/rockNormals.jpg";
	asteroidMat->SetNormalMap(device, context, path);

	path = L"Assets/textures/title.png";
	menuMat = new Material(device, context, path);

	path = L"Assets/textures/creditsTexture.jpg";
	creditsMat = new Material(device, context, path);


	path = L"Assets/textures/spaceBackground.dds";
	backgroundMat = new Material(device, context, path, 0);

	path = L"Assets/textures/venus.png";
	venusMat = new Material(device, context, path);

	path = L"Assets/textures/sun.jpg";
	sunMat = new Material(device, context, path);

	path = L"Assets/textures/earth.jpg";
	earthMat = new Material(device, context, path);

	path = L"Assets/textures/moon.jpg";
	moonMat = new Material(device, context, path);

	path = L"Assets/textures/planet1.jpg";
	p1Mat = new Material(device, context, path);

	path = L"Assets/textures/planet2.png";
	p2Mat = new Material(device, context, path);

	path = L"Assets/textures/planet3.png";
	p3Mat = new Material(device, context, path);

	path = L"Assets/textures/rainbow3.png";
	laneMat = new Material(device, context, path);
	laneMat->UseTransperancy(true);

	path = L"Assets/textures/blueSpace3.png";
	lane2Mat = new Material(device, context, path);
	lane2Mat->UseTransperancy(true);

	path = L"Assets/Textures/circleParticle.jpg"; 
	particelMat = new Material(device, context, path, 1);

	path = L"Assets/Textures/titleText.png";
	titleMat = new Material(device, context, path);

}

//---------------------------------------------------------
//Setup lights
//---------------------------------------------------------
void SceneBuilder::BuildLights()
{
	//global ambient light
	ambient = new GlobalLight();
	ambient->AmbientColor = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

	//Directional Lights
	//-----------------------------------------------------------

	//This light is for a no light place holder to be used if a dir light was used before
	//and you don't need one in the current scene.
	dirLight = new DirectionalLight();
	dirLight->DiffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLight->Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

	dirLight2 = new DirectionalLight();
	dirLight2->DiffuseColor = XMFLOAT4(.3f, .3f, .3f, 1);
	dirLight2->Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

	dirLight3 = new DirectionalLight();
	dirLight3->DiffuseColor = XMFLOAT4(.6f, .6f, .6f, 1.0f);
	dirLight3->Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

	dirLight4 = new DirectionalLight();
	dirLight4->DiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	dirLight4->Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

	//Point Lights
	//-----------------------------------------------------------
	pointLight = new PointLight();
	pointLight->DiffuseColor = XMFLOAT4(0.6f, 1.0f, 1.0f, 1.0f);
	pointLight->Position = XMFLOAT3(20.0f, 20.0f, 20.0f);

	//Spot Lights
	//-----------------------------------------------------------
	spotLight = new SpotLight();
	spotLight->DiffuseColor = XMFLOAT4(0.8f, 0.3f, 0.0f, 1);
	spotLight->Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	spotLight->phi = 0.0f;
	spotLight->Position = XMFLOAT3(0.0f, 15.0f, 0.0f);
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

	titleEnt = new Entity(quadMesh, titleMat, XMFLOAT3(+0.0f, +5.0f, +0.0f), XMFLOAT3(+0.2f, +0.0f, +0.0f), XMFLOAT3(+13.0f, +1.0f, +1.0f));

	playerEnt = new Entity(playerMesh, playerMat, XMFLOAT3(0.0f, -1.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));

	for (int i = 0; i < 12; i++) {
		asteroidList[i]= new Entity(asteroidMesh, asteroidMat, XMFLOAT3(2.0f, 1.5f, -10.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+2.0f, +2.0f, +2.0f));
	}

	laneEnt = new Entity(quadMesh, lane2Mat, XMFLOAT3(-2.8f, -1.2f, 8.0f), XMFLOAT3(+1.6f, -0.1f, +0.0f), XMFLOAT3(+1.0f, +14.0f, +1.0f));
	laneEnt2 = new Entity(quadMesh, laneMat, XMFLOAT3(1.5f, -1.2f, 8.0f), XMFLOAT3(+1.6f, -0.1f, +0.0f), XMFLOAT3(+1.0f, +14.0f, +1.0f));

	menuBackgroundEnt = new Entity(quadMesh, menuMat, XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+20.0f, +20.0f, +1.0f));
	gameBackgroundEnt = new Entity(skyboxMesh, backgroundMat, XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+10.0f, +10.0f, +10.0f));
	creditsBackgroundEnt = new Entity(quadMesh, creditsMat, XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+20.0f, +20.0f, +1.0f));


	venusEnt = new Entity(sphereMesh, venusMat, XMFLOAT3(-60, -10, 30), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+100.0f, +100.0f, +100.0f));
	sunEnt = new Entity(sphereMesh, sunMat, XMFLOAT3(30, -10, 40), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+10.0f, +10.0f, +10.0f));
	earthEnt = new Entity(sphereMesh, earthMat, XMFLOAT3(1.0f, 0, 0), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+0.5f, +0.5f, +0.5f));
	moonEnt = new Entity(sphereMesh, moonMat, XMFLOAT3(1.0f, 0, 0), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+0.3f, +0.3f, +0.3f));
	p1= new Entity(sphereMesh, p1Mat, XMFLOAT3(0, 0, 1.5f), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+0.7f, +0.7f, +0.7f));
	p2 = new Entity(sphereMesh, p2Mat, XMFLOAT3(-1.5f, 0, 0), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+0.3f, +0.3f, +0.3f));
	p3 = new Entity(sphereMesh, p3Mat, XMFLOAT3(-1.5f, 0, -1), XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(+0.6f, +0.6f, +0.6f));

	p1->SetParent(sunEnt);
	p2->SetParent(sunEnt);
	p3->SetParent(sunEnt);
	earthEnt->SetParent(sunEnt);
	moonEnt->SetParent(earthEnt);
}

void SceneBuilder::BuildParticles()
{
	emitter = new Emitter(
		200,							// Max particles
		200,							// Particles per second
		0.5f,							// Particle lifetime
		0.25f,							// Start size
		2.0f,							// End size
		XMFLOAT4(1, 0.1f, 0.1f, 0.2f),	// Start color
		XMFLOAT4(1, 1.0f, 1.0f, 0),		// End color
		XMFLOAT3(10, 10, 0),			// Start velocity
		XMFLOAT3(0, 0, 0),				// Start position
		XMFLOAT3(10, 10, 0),			// Start acceleration
		device,
		particelMat);
}

//---------------------------------------------------------
//Setup the 3 built in scenes here
//---------------------------------------------------------
void SceneBuilder::SetupScenes()
{
	//Scene 1
	//--------------------------------------------------------------------------------------------
	scene1 = new Scene();
	scene1->name = "Menu";

	//Entities
	scene1->entities = std::vector<Entity*>();
	scene1->opaque = std::vector<Entity*>();
	scene1->opaqueNorm = std::vector<Entity*>();
	scene1->transparent = std::vector<Entity*>();
	scene1->transparentNorm = std::vector<Entity*>();
	scene1->textList = std::vector<textObject>();

	//Background
	scene1->entities.push_back(menuBackgroundEnt);
	scene1->entities.push_back(titleEnt);

	//Lights
	scene1->globalLights.push_back(ambient);
	scene1->directionalLights.push_back(dirLight4);

	//Text
	scene1->textList.push_back(textObject{ L"press ENTER to start!", DirectX::XMFLOAT2(500, 550) });
	
	//Sound
	//scene1->musicFileName = "";
	
	//End of Scene 1 -----------------------------------------------------------------------------

	//Scene 2
	//--------------------------------------------------------------------------------------------
	scene2 = new Scene();
	scene2->name = "MainGame";

	//Entities
	scene2->entities = std::vector<Entity*>();
	scene2->opaque = std::vector<Entity*>();
	scene2->opaqueNorm = std::vector<Entity*>();
	scene2->transparent = std::vector<Entity*>();
	scene2->transparentNorm = std::vector<Entity*>();
	scene2->textList = std::vector<textObject>();
	
	for (Entity* e : asteroidList) {
		scene2->entities.push_back(e);
	}

	scene2->entities.push_back(playerEnt);

	scene2->entities.push_back(laneEnt);
	scene2->entities.push_back(laneEnt2);
	scene2->entities.push_back(venusEnt);
	scene2->entities.push_back(sunEnt);
	scene2->entities.push_back(earthEnt);
	scene2->entities.push_back(moonEnt);
	scene2->entities.push_back(p1);
	scene2->entities.push_back(p2);
	scene2->entities.push_back(p3);
	scene2->sun = sunEnt;
	scene2->moon = moonEnt;
	scene2->earth = earthEnt;
	scene2->venus = venusEnt;

	scene2->planet1 = p1;
	scene2->planet2 = p2;
	scene2->planet3 = p3;

	//Background
	scene2->background = gameBackgroundEnt;

	//Lights
	scene2->globalLights.push_back(ambient);

	scene2->pointLights.push_back(pointLight);

	scene2->directionalLights.push_back(dirLight);

	//Sound
	scene2->musicFileName = "04_-_Bloody_Revenge.mp3";

	//Particles
	scene2->Particles = emitter;

	//2D Text
	scene2->textList.push_back(textObject{ L"Score: ", DirectX::XMFLOAT2(500, 10) });

	//End of Scene 2 -----------------------------------------------------------------------------

	//Scene 3
	//--------------------------------------------------------------------------------------------
	scene3 = new Scene();
	scene3->name = "Results";

	//Entities
	scene3->entities = std::vector<Entity*>();
	scene3->opaque = std::vector<Entity*>();
	scene3->opaqueNorm = std::vector<Entity*>();
	scene3->transparent = std::vector<Entity*>();
	scene3->transparentNorm = std::vector<Entity*>();
	scene3->textList = std::vector<textObject>();

	//Background
	scene3->entities.push_back(creditsBackgroundEnt);

	//Lights
	scene3->globalLights.push_back(ambient);
	scene3->directionalLights.push_back(dirLight4);

	//2D Text
	scene3->textList.push_back(textObject{ L"Results", DirectX::XMFLOAT2(580, 300) });
	scene3->textList.push_back(textObject{ L"You Scored: ", DirectX::XMFLOAT2(530, 350) });

	//End of Scene 3 -----------------------------------------------------------------------------

	//Scene 4
	//--------------------------------------------------------------------------------------------
	scene4 = new Scene();
	scene4->name = "Credits";

	//Entities
	scene4->entities = std::vector<Entity*>();
	scene4->opaque = std::vector<Entity*>();
	scene4->opaqueNorm = std::vector<Entity*>();
	scene4->transparent = std::vector<Entity*>();
	scene4->transparentNorm = std::vector<Entity*>();
	scene4->textList = std::vector<textObject>();

	//Background
	scene4->entities.push_back(creditsBackgroundEnt);

	//Lights
	scene4->globalLights.push_back(ambient);
	scene4->directionalLights.push_back(dirLight4);
	
	//Sound
	scene4->musicFileName = "06_-_Hypnagogic.mp3";

	//2D Text
	scene4->textList.push_back(textObject{ L"Credits", DirectX::XMFLOAT2(600, 10) });

	scene4->textList.push_back(textObject{ L"Designers & Developers", DirectX::XMFLOAT2(500, 50) });
	scene4->textList.push_back(textObject{ L"Shih-Kuang Chu", DirectX::XMFLOAT2(550, 90) });
	scene4->textList.push_back(textObject{ L"Darren Farr", DirectX::XMFLOAT2(580, 130) });
	scene4->textList.push_back(textObject{ L"Rahul Madhyan", DirectX::XMFLOAT2(560, 170) });
	scene4->textList.push_back(textObject{ L"Pengfei Zhang", DirectX::XMFLOAT2(562, 210) });

	scene4->textList.push_back(textObject{ L"Models provided by Chris Cascioli", DirectX::XMFLOAT2(430, 300) });
	scene4->textList.push_back(textObject{ L"Textures by Google Search", DirectX::XMFLOAT2(450, 350) });

	scene4->textList.push_back(textObject{ L"Sound from FreeMusicArchive.org", DirectX::XMFLOAT2(430, 400) });
	scene4->textList.push_back(textObject{ L"Bloody Revenge", DirectX::XMFLOAT2(580, 450) });
	scene4->textList.push_back(textObject{ L"Hypnagogic", DirectX::XMFLOAT2(580, 500) });

	//End of Scene 4 -----------------------------------------------------------------------------

	//Sort the entities in the scenes
	SortEntityList(scene1);
	SortEntityList(scene2);
	SortEntityList(scene3);
	SortEntityList(scene4);
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
//Sort the entities lists into opaque or transparent lists, with or without normal maps
//---------------------------------------------------------
void SceneBuilder::SortEntityList(Scene* s)
{

	for (unsigned int i = 0; i < s->entities.size(); i++)
	{
		if (s->entities.at(i)->GetMat()->UseTransperancy() && s->entities.at(i)->GetMat()->HasNormalMap())
		{
			s->transparentNorm.push_back(s->entities.at(i));
		}
		else if (s->entities.at(i)->GetMat()->UseTransperancy())
		{
			s->transparent.push_back(s->entities.at(i));
		}
		else if (s->entities.at(i)->GetMat()->HasNormalMap())
		{
			s->opaqueNorm.push_back(s->entities.at(i));
		}
		else
		{
			s->opaque.push_back(s->entities.at(i));
		}
	}

}

//---------------------------------------------------------
//Return the list of entities in the scene
//---------------------------------------------------------
Scene* SceneBuilder::GetScene(int num)
{
	if (num == 1) { return scene1; }
	else if (num == 2) { return scene2; }
	else if (num == 3) { return scene3; }
	else if (num == 4) { return scene4; }
	else { return nullptr; }
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

