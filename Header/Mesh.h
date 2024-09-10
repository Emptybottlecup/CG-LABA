#pragma once
#include "GameComponent.h"

struct VertexPos;

class Mesh : public GameComponent
{
public:
	Mesh(Game* GameInstance, std::vector<VertexPos>& vertices, std::vector<DWORD>& indices, std::wstring texturePath);

	void Initialize();

	void Update(float deltaTime);

	void Draw();

	void DestroyResources();

	~Mesh();
private:

	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11ShaderResourceView* pTextureRV;

	std::wstring pTexturePath;

	std::vector <VertexPos> pVertices;
	std::vector <DWORD> pIndices;
};