//Darren Farr
#pragma once
#include <vector>
#include "Scene.h"

//---------------------------------------------------------
//Scene Manager class
//Contains one or more scenes made up of entities, made of meshes and materials
//---------------------------------------------------------
class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	Scene* GetScene(int);
	void AddScene(Scene*);
	void RemoveScene(int);
	unsigned int GetNumOfScenes();

private:
	std::vector<Scene*>Scenes;
	unsigned int numOfScenes;
};