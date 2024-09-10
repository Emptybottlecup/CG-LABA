#pragma once
#include "Mesh.h"

class CharacterBall;

struct Material;

class Model: public GameComponent
{
public:

	Model(Game* GameInstance, const std::string& filename);

	Model(Game* GameInstance, const std::string& filename, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation);
	void Initialize();

	virtual void Update(float deltaTime);

	DirectX::BoundingSphere GetCollision();

	void SetParent(CharacterBall* character);

	void Draw();

	void DestroyResources();

	~Model();

protected:

	bool LoadModel(const std::string& filename);

	void ProcessNode(aiNode* node, const aiScene* scene);

	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11InputLayout* pInputLayout;

	ID3D11ShaderResourceView* pTextureRV;
	ID3D11SamplerState* pSamplerLinear;

	DirectX::XMFLOAT3 pPosition;
	DirectX::XMFLOAT3 pRotation;
	DirectX::XMFLOAT3 pScale;
	DirectX::XMVECTOR pQuaternion;

	std::vector<Mesh*> pMeshes;
	const std::string pFilename;

	DirectX::BoundingSphere pCollision;
	CharacterBall* pCharacter;
	DirectX::XMVECTOR pDistanceForCharacter;


	DirectX::XMVECTOR initialRelativeRotation;

	DirectX::XMFLOAT3 Ambient;
	float Shininess;
	DirectX::XMFLOAT3 Diffuse;
	DirectX::XMFLOAT3 Specular;
};