#include "Player.h"

using namespace DirectX;

Player::Player()
{
	
}

Player::Player(Entity* playerEnt) : playerEntity(playerEnt)
{
	playerCollider = new Collider(playerEntity->GetMesh());
}

Player::~Player()
{
	if(playerCollider != NULL)
		delete playerCollider;
}

void Player::Update(float deltaTime)
{
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000))
	{
		holdTimer += deltaTime;
		if (holdTimer <= 0.2f) {
			XMFLOAT3 pos(-1.0f, 0.0f, 0.0f);
			playerEntity->SetPosition(pos);
		}
		else {
			XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
			playerEntity->SetPosition(pos);
		}	

	}

	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		holdTimer += deltaTime;
		if (holdTimer <= 0.2f) {
			XMFLOAT3 pos(1.0f, 0.0f, 0.0f);
			playerEntity->SetPosition(pos);
		}
		else {
			XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
			playerEntity->SetPosition(pos);
		}
	}
	else {
		holdTimer = 0;
		XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
		playerEntity->SetPosition(pos);
	}
}

void Player::Move(float x, float y, float z, float deltaTime)
{
	XMVECTOR moveX = XMVectorZero();
	XMVECTOR moveY = XMVectorZero();
	XMVECTOR moveZ = XMVectorZero();
	float moveAmount = 5.0f * deltaTime;

	XMVECTOR fwd = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR right = XMVectorSet(1, 0, 0, 0);

	XMVECTOR pos = XMLoadFloat3(&(playerEntity->GetPosition()));

	moveX = XMVector3Cross(fwd, up);
	moveY = XMVector3Cross(fwd, right);
	moveZ = XMVector3Cross(up, right);
	moveX = moveX * moveAmount;
	moveY = moveY* moveAmount;
	moveZ = moveZ*moveAmount;

	pos += moveX*x + moveY*y + moveZ*z;

	XMFLOAT3 position;
	XMStoreFloat3(&position, pos);

	playerEntity->SetPosition(position);
}

Collider* Player::GetCollider()
{
	return playerCollider;
}