#include "../Header/Camera.h"
#include <iostream>
#include "../Header/CharacterBall.h"


Camera::Camera(DirectX::XMFLOAT3 startPosition, HWND* hWnd, InputDevice* inputdevice) : phWnd(hWnd), pInput(inputdevice)
{
	pPosition = startPosition;
	pFront = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	pUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

	pPositionVector = DirectX::XMLoadFloat3(&pPosition);
	pFrontVector = DirectX::XMLoadFloat3(&pFront);
	pUpVector = DirectX::XMLoadFloat3(&pUp);

	yaw = 0.0f;
	pitch = 0.0f;

	localRotate = DirectX::XMQuaternionIdentity();

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	CenterX = screenWidth / 2;
	CenterY = screenHeight / 2;

	pDistanceForCharacter = DirectX::XMVectorZero();
}

Camera::Camera(HWND* hWnd, InputDevice* inputdevice) : phWnd(hWnd), pInput(inputdevice)
{
	pPosition = DirectX::XMFLOAT3(0.0f, 0.0f, -100.0f);
	pFront = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	pUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	pRight = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);

	pPositionVector = DirectX::XMLoadFloat3(&pPosition);
	pFrontVector = DirectX::XMLoadFloat3(&pFront);
	pUpVector = DirectX::XMLoadFloat3(&pUp);
	pRightVector = DirectX::XMLoadFloat3(&pRight);

	yaw = 0.0f;
	pitch = 0.0f;

	localRotate = DirectX::XMQuaternionIdentity();

	pWidth = GetSystemMetrics(SM_CXSCREEN);
	pHeight = GetSystemMetrics(SM_CYSCREEN);

	CenterX = pWidth / 2;
	CenterY = pHeight / 2;

	pDistanceForCharacter = DirectX::XMVectorZero();
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return DirectX::XMMatrixLookToRH(pPositionVector, pFrontVector, pUpVector);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{

	return DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(74.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
}

DirectX::XMVECTOR Camera::GetPositionVector()
{
	return pPositionVector;
}

DirectX::XMVECTOR Camera::GetFrontVector()
{
	return pFrontVector;
}

DirectX::XMVECTOR Camera::GetRightVector()
{
	return pRightVector;
}

float Camera::GetCameraSpeed()
{
	return pCameraspeed;
}

void Camera::ProcessTransformPosition(float deltaTime)
{
	ShowCursor(false);

	pFrontVector = DirectX::XMLoadFloat3(&pFront);
	pUpVector = DirectX::XMLoadFloat3(&pUp);

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	float deltaX = static_cast<float>(cursorPos.x - CenterX) * sensitivity;
	float deltaY = static_cast<float>(cursorPos.y - CenterY) * sensitivity;

	yaw -= deltaX;
	pitch -= deltaY;

	if (pitch > 82.0f) {
		pitch = 82.0f;
	}
	else if (pitch < -7.0f) {
		pitch = -7.0f;
	}

	DirectX::XMVECTOR quatYaw = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMConvertToRadians(yaw));
	DirectX::XMVECTOR quatPitch = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMConvertToRadians(pitch));

	localRotate = DirectX::XMQuaternionMultiply(quatPitch, quatYaw);

	pFrontVector = DirectX::XMVector3Rotate(pFrontVector, localRotate);
	pFrontVector = DirectX::XMVector3Normalize(pFrontVector);

	pUpVector = DirectX::XMVector3Rotate(pUpVector, localRotate);
	pUpVector = DirectX::XMVector3Normalize(pUpVector);

	pRightVector = DirectX::XMVector3Cross(pUpVector, pFrontVector);
	pRightVector = DirectX::XMVector3Normalize(pRightVector);

	if (pCharacter != nullptr)
	{
		DirectX::XMVECTOR targetPosition = DirectX::XMVectorAdd(DirectX::XMVector3Rotate(pDistanceForCharacter, localRotate), DirectX::XMLoadFloat3(pCharacter->GetPosition()));
		pPositionVector = targetPosition;
		DirectX::XMStoreFloat3(&pPosition, pPositionVector);
	}
	else
	{
		if (pInput->IsKeyDown(Keys::W))
		{

			pPositionVector = DirectX::XMVectorAdd(pPositionVector, DirectX::XMVectorScale(pFrontVector, pCameraspeed * deltaTime));
		}
		if (pInput->IsKeyDown(Keys::S))
		{
			pPositionVector = DirectX::XMVectorSubtract(pPositionVector, DirectX::XMVectorScale(pFrontVector, pCameraspeed * deltaTime));
		}
		if (pInput->IsKeyDown(Keys::A))
		{
			pPositionVector = DirectX::XMVectorAdd(pPositionVector, DirectX::XMVectorScale(pRightVector, pCameraspeed * deltaTime));
		}
		if (pInput->IsKeyDown(Keys::D))
		{
			pPositionVector = DirectX::XMVectorSubtract(pPositionVector, DirectX::XMVectorScale(pRightVector, pCameraspeed * deltaTime));
		}
		if (pInput->IsKeyDown(Keys::Space))
		{
			pPositionVector = DirectX::XMVectorAdd(pPositionVector, DirectX::XMVectorScale(pUpVector, pCameraspeed * deltaTime));
		}
		if (pInput->IsKeyDown(Keys::X))
		{
			pPositionVector = DirectX::XMVectorSubtract(pPositionVector, DirectX::XMVectorScale(pUpVector, pCameraspeed * deltaTime));
		}
	}

	SetCursorPos(pWidth / 2, pHeight / 2);
}

void Camera::SetCharacter(CharacterBall* character)
{
	pCharacter = character;
	pPosition = *(pCharacter->GetPosition());

	pPosition.z -= 10;

	pPositionVector = DirectX::XMLoadFloat3(&pPosition);

	pDistanceForCharacter = DirectX::XMVectorSubtract(pPositionVector, DirectX::XMLoadFloat3(pCharacter->GetPosition()));
}