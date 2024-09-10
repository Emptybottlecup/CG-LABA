#pragma once
#include "Keys.h"
#include "Game.h"

class Game;

class GameComponent
{
public:
	GameComponent(Game* GameObject);

	virtual void Initialize() = 0;

	bool CompileD3DShader(const char* filePath, const char* entry, const char* shaderModel, ID3DBlob** buffer);

	virtual void Draw() = 0;
	virtual void DestroyResources() = 0;
	virtual void Update(float deltaTime) = 0;

	virtual void Reload();
protected:
	Game* pGame;
};



class TriangleGameComponent : public GameComponent
{
public:

	TriangleGameComponent(Game* GameObject, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices, DirectX::XMFLOAT4 offset);

	void Initialize();

	void Draw();

	void Update(float deltaTime);

	void DestroyResources();

	~TriangleGameComponent();

private:
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;

	ID3D11InputLayout* pInputLayout;
	ID3D11Buffer* pVertices;
	ID3D11Buffer* pIndexBuffer;

	std::vector<DirectX::XMFLOAT4> pPoints;
	std::vector<int> pIndices;
};
