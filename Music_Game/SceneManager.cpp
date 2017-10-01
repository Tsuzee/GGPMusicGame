//Darren Farr
#include "SceneManager.h"

//---------------------------------------------------------
//Default Constructor
//---------------------------------------------------------
SceneManager::SceneManager()
{
	numOfScenes = 0;
}

//---------------------------------------------------------
//Default Deconstructor
//---------------------------------------------------------
SceneManager::~SceneManager()
{

}

//---------------------------------------------------------
//Return the list of entities in the scene
//---------------------------------------------------------
Scene* SceneManager::GetScene(int num)
{
	return Scenes.at(num - 1);
}

//---------------------------------------------------------
//Add a scene to the list
//---------------------------------------------------------
void SceneManager::AddScene(Scene* s)
{
	Scenes.push_back(s);
	numOfScenes++;
}

//---------------------------------------------------------
//Remove a scene from the list
//---------------------------------------------------------
void SceneManager::RemoveScene(int num)
{
	std::vector<Scene*>temp;
	for (int i = 0; i < Scenes.size(); i++)
	{
		if (i != num)
		{
			temp.push_back(Scenes.at(i));
		}
	}

	Scenes = temp;
	numOfScenes--;
}

//---------------------------------------------------------
//Returns the number of scenes
//---------------------------------------------------------
unsigned int SceneManager::GetNumOfScenes()
{
	return numOfScenes;
}