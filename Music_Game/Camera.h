//Darren Farr
#pragma once

#include <DirectXMath.h>
#include <Windows.h>


class Camera
{
public:
	Camera();
	Camera(unsigned int, unsigned int);
	~Camera();

	void Init();

	void SetWidthHeight(unsigned int, unsigned int);
	void Update(POINT, float);
	void MouseDown(bool);

	void Resize(unsigned int, unsigned int);

	void SetDebugMode(bool);

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4 GetPositon();
	DirectX::XMFLOAT4 GetDirection();

private:
	void CreateMatrices(DirectX::XMFLOAT4, DirectX::XMFLOAT4);

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 direction;
	float rotationX;
	float rotationY;

	unsigned int width;
	unsigned int height;

	bool mouseDown;
	bool debugMode;

	POINT lastMouse, mouse;
};