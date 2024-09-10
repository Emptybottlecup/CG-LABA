#pragma once
#include "GameComponent.h"

class Cube : public GameComponent
{
public:
	Cube(Game* GameInstance);

	Cube(Game* GameInstance, const DirectX::XMFLOAT3& Position);

	Cube(Game* GameInstance, Cube* Parent, const DirectX::XMFLOAT3& Position, const DirectX::XMFLOAT3& Scale, const DirectX::XMFLOAT3& Rotation, const float speed);

	void Initialize();

	void Update(float deltaTime);

	DirectX::XMFLOAT3* GetPosition();

	void Draw();

	void DestroyResources();

	~Cube();
private:
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;

	ID3D11InputLayout* pInputLayout;
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;

	DirectX::XMFLOAT3 pPosition;
	DirectX::XMFLOAT3 pRotation;
	DirectX::XMFLOAT3 pScale;
	DirectX::XMVECTOR pQuaternion;

	DirectX::XMVECTOR pDistanceFromParent;
	float pAngle = 0.0f;
	float pSpeed = 0.0;
	Cube* pParent = nullptr;
};