//Darren Farr
#pragma once
#include "SimpleShader.h"
#include "Entity.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include "Lights.h"
#include "Scene.h"
#include "Emitter.h"

//---------------------------------------------------------
//Scene Manager class
//Contains one or more scenes made up of entities, made of meshes and materials
//---------------------------------------------------------
class SceneBuilder
{
public:
	SceneBuilder();
	~SceneBuilder();

	void Init(ID3D11Device*, ID3D11DeviceContext*);
	void BuildMaterials();
	void BuildLights();
	void BuildMeshes();
	void BuildEntities();
	void BuildParticles();

	Entity* CreateEntity(Mesh*, Material*, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3);
	Scene* GetScene(int);
	Entity* GetPlayerEntity();

	Entity* GetAsteroidEntity(int);

private:
	void SetupScenes();
	void SortEntityList(Scene*);

	Scene* scene1;
	Scene* scene2;
	Scene* scene3;
	Scene* scene4;

	ID3D11Device* device;
	ID3D11DeviceContext* context;

	Mesh* cubeMesh;
	Mesh* quadMesh;
	Mesh* sphereMesh;
	Mesh* playerMesh;
	Mesh* asteroidMesh;
	Mesh* skyboxMesh;
	
	Material* creditsMat;
	Material* menuMat;
	Material* playerMat;
	Material* laneMat;
	Material* asteroidMat;
	Material* backgroundMat;

	Material* plutoMat;
	Material* venusMat;
	Material* sunMat;
	Material* earthMat;
	Material* moonMat;
	Material* p1Mat;
	Material* p2Mat;
	Material* p3Mat;
	Material* lane2Mat;
	Material* titleMat;

	Material* particelMat;

	Entity* menuEnt;
	Entity* titleEnt;
	Entity* menuBackgroundEnt;
	Entity* gameBackgroundEnt;
	Entity* creditsBackgroundEnt;
	Entity* playerEnt;


	Entity* laneEnt;
	Entity* laneEnt2;
	Entity* plutoEnt;
	Entity* venusEnt;
	Entity* sunEnt;
	Entity* moonEnt;
	Entity* earthEnt;
	Entity* p1;
	Entity* p2;
	Entity* p3;

	GlobalLight* ambient;

	DirectionalLight* dirLight;
	DirectionalLight* dirLight2;
	DirectionalLight* dirLight3;
	DirectionalLight* dirLight4;

	PointLight* pointLight;

	SpotLight* spotLight;


	Entity* asteroidList[12];

	//Particles Emitter
	Emitter *emitter;

};