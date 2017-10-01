#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Entity.h"
#include "Collider.h"

class Player
{
public:
	Player();
	Player(Entity* playerEnt);
	~Player();

	Collider* GetCollider();
	void Update(float deltaTime);

private:
	Entity* playerEntity;
	Collider* playerCollider;
	void Move(float x, float y, float z, float deltaTime);
	float holdTimer=0;
	bool holdFlag=false;
};

