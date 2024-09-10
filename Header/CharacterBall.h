#pragma once
#include "GameComponent.h"
#include <DirectXCollision.h>

class Camera;

class CharacterBall : public GameComponent
{
public:
	CharacterBall(Game* GameInstance, int radius, int latitudeSegments, int longitudeSegments, const wchar_t* filename, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotattion);

	void Initialize();

	void Update(float deltaTime);

	DirectX::XMFLOAT3* GetPosition();

	void SetCamera(Camera* camera);

	DirectX::BoundingSphere* GetCollision();

	DirectX::XMVECTOR GetQuaternion();

	void Draw();

	void DestroyResources();

	~CharacterBall();
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

	Camera* pCamera = nullptr;
	DirectX::BoundingSphere pCollision;
};

/*
#pragma once
#include "Model.h"

#include <DirectXCollision.h>

class Camera;

class CharacterBall : public Model
{
public:
	CharacterBall(Game* GameInstance, const std::string& filenamel, const wchar_t* textureFilename, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, int radius);
	void Update(float deltaTime);

	DirectX::XMFLOAT3* GetPosition();

	void SetCamera(Camera* camera);

	DirectX::BoundingSphere* GetCollision();

	DirectX::XMVECTOR GetQuaternion();

	~CharacterBall();
private:
	int pRadius;
	Camera* pCamera = nullptr;
	DirectX::BoundingSphere pCollision;
};
*/