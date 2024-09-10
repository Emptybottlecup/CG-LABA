#pragma once 
#include "Game.h"
#include "InputDevice.h"


class CharacterBall;

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 startPosition, HWND* hWnd, InputDevice* inputdevice);
	Camera(HWND* hwnd, InputDevice* input);
	~Camera() = default;

	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetProjectionMatrix();

	DirectX::XMVECTOR GetPositionVector();

	DirectX::XMVECTOR GetFrontVector();

	DirectX::XMVECTOR GetRightVector();

	float GetCameraSpeed();
	void ProcessTransformPosition(float deltaTime);

	void SetCharacter(CharacterBall* character);

private:
	float pCameraspeed = 10.0f;
	float sensitivity = 0.1f;
	HWND* phWnd;
	InputDevice* pInput;

	DirectX::XMFLOAT3 pPosition;
	DirectX::XMFLOAT3 pFront;
	DirectX::XMFLOAT3 pUp;
	DirectX::XMFLOAT3 pRight;

	DirectX::XMVECTOR pPositionVector;
	DirectX::XMVECTOR pFrontVector;
	DirectX::XMVECTOR pUpVector;
	DirectX::XMVECTOR pRightVector;
	DirectX::XMVECTOR localRotate;

	float yaw = 0.0f;
	float pitch = 0.0f;
	int pWidth;
	int pHeight;
	float CenterX = 0;
	float CenterY = 0;

	CharacterBall* pCharacter = nullptr;
	DirectX::XMVECTOR pDistanceForCharacter;
};