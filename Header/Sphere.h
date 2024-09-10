#pragma once
#include "GameComponent.h"

class Sphere : public GameComponent
{
public:
	Sphere(Game* GameInstance, int radius, int latitudeSegments, int longitudeSegments, Sphere* parent, const DirectX::XMFLOAT3& position, const int speed);

	Sphere(Game* GameInstance, int radius, int latitudeSegments, int longitudeSegments, Sphere* parent, const DirectX::XMFLOAT3& position, const int speed, const wchar_t* textureFilename);

	float GetSpeed();
	void Initialize();

	void Update(float deltaTime);

	DirectX::XMFLOAT3* GetPosition();

	void Draw();

	void DestroyResources();

	~Sphere();
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

	ID3D11ShaderResourceView* pTextureRV;
	ID3D11SamplerState* pSamplerLinear;
	const wchar_t* pTextureFilename;

	int pRadius;
	int pLatitudeSegments;
	int pLongitudeSegments;
	int pIndexCount = 0;

	DirectX::XMVECTOR pDistanceFromParent;
	Sphere* pParent = nullptr;
	float pAngle = 0.0f;
	float pSpeed = 0.0;
};