#pragma once
#include "GameComponent.h"
#include <DirectXCollision.h>


class Pong;

class BoxGameComponent : public GameComponent
{
public:
	
	BoxGameComponent(Game* GameObject, Pong* pong);

	BoxGameComponent(Game* GameObject, Pong* pong, std::vector<DirectX::XMFLOAT4> points);

	void Initialize();

	void Draw();

	virtual void Update(float deltaTime);

	DirectX::BoundingBox GetCollision()
	{
		return boundingBox;
	}

	void DestroyResources();

	~BoxGameComponent();

protected:
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;

	ID3D11InputLayout* pInputLayout;
	ID3D11Buffer* pVertices;

	std::vector<DirectX::XMFLOAT4> pPoints;

	DirectX::BoundingBox boundingBox;

	Pong* pPong;
};