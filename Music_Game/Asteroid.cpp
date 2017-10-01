#include "Asteroid.h"
using namespace DirectX;
#include "Game.h"

Asteroid::Asteroid()
{
}


Asteroid::~Asteroid()
{
	if(asterCollider != NULL)
		delete asterCollider;
}

Asteroid::Asteroid(Entity * asterEntity)
{
	this->asterEntity = asterEntity;

	asterCollider = new Collider(asterEntity->GetMesh());
	active = false;
}

void Asteroid::Update(float deltaTime)
{
	if (active)
	{
		Move(-0.2f, 0, 2, deltaTime);
		XMVECTOR pos = XMLoadFloat3(&(asterEntity->GetPosition()));
		XMFLOAT3 position;
		XMFLOAT3 rota = asterEntity->GetRotation();
		rota.y += (rand() % 3)*0.005f;
		rota.x += (rand() % 3)*0.005f;
		rota.z += (rand() % 3)*0.005f;
		//rota.y += 0.005f;
		asterEntity->SetRotation(rota);


		XMStoreFloat3(&position, pos);
		if (position.z < -10) {
			active = false;
		}

		if (position.z < -1.0f) {
			collided = true;			
		}

	}
}

void Asteroid::RandomPos() {
	XMVECTOR pos = XMVectorSet(rand()%4-2, rand() % 5 - 2, 20, 0);
	XMFLOAT3 position;
	XMStoreFloat3(&position, pos);
	asterEntity->SetPosition(position);
}

void Asteroid::Move(float x, float y, float z,float deltaTime)
{
	XMVECTOR moveX = XMVectorZero();
	XMVECTOR moveY = XMVectorZero();
	XMVECTOR moveZ = XMVectorZero();
	float moveAmount = 5.0f * deltaTime;

	XMVECTOR fwd = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR right = XMVectorSet(1, 0, 0, 0);

	XMVECTOR pos = XMLoadFloat3(&(asterEntity->GetPosition()));

	moveX = XMVector3Cross(fwd, up);
	moveY = XMVector3Cross(fwd, right);
	moveZ = XMVector3Cross(up, right);
	moveX = moveX * moveAmount;
	moveY = moveY * moveAmount;
	moveZ = moveZ * moveAmount;
	

	pos += moveX*x + moveY*y + moveZ*z;

	XMFLOAT3 position;
	XMStoreFloat3(&position, pos);

	asterEntity->SetPosition(position);
}

Collider* Asteroid::GetCollider()
{
	return asterCollider;
}

// --------------------------------------------------------
//Set the asteroid to active and its lane to move to (1,2,3)
// --------------------------------------------------------
void Asteroid::SetActive(int laneNumber)
{
	collided = false;
	XMFLOAT3 position;
	active = true;

	switch (laneNumber)
	{
	case 1:
	{
		position = XMFLOAT3(-4.0f, 0.0f, 20.0f);
		asterEntity->SetPosition(position);
		break;
	}
	case 2:
	{
		position = XMFLOAT3(+0.1f, 0.0f, 20.0f);
		asterEntity->SetPosition(position);
		break;
	}
	default:
		break;
	}
}

// --------------------------------------------------------
//Is the Asteroid active?
// --------------------------------------------------------
bool Asteroid::IsActive()
{
	return active;
}