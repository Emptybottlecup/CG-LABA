#include "../Header/GameStick.h"
#include "iostream"
#include "../Header/Pong.h"

GameStick::GameStick(Game* GameObject, const DirectX::XMFLOAT2& position, Pong* pong) : BoxGameComponent(GameObject, pong), pPosition(position)
{
	std::vector<DirectX::XMFLOAT4> Points =
	{
		DirectX::XMFLOAT4(position.x - 0.01f,position.y + 0.1f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + 0.01f,position.y + 0.1f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x - 0.01f, position.y - 0.1f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + 0.01f, position.y - 0.1f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};

	pPoints = Points;

    boundingBox.Center = { pPosition.x, pPosition.y, 0 };
    boundingBox.Extents = { 0.01f, 0.1f,0 };
}

void GameStick::Update(float deltaTime)
{
    switch (pState) {

    case Direction::UP:
        if ((pPosition.y + 0.1) * size.y <= 1.0)
            pPosition.y += deltaTime * pSpeed;
        break;

    case Direction::DOWN:
        if ((pPosition.y - 0.1) * size.y >= -1.0)
            pPosition.y -= deltaTime * pSpeed;
        break;

    default:
        break;
    }

    boundingBox.Center = {pPosition.x, pPosition.y, 0};

	pPong->ChangeConstantBufferPong(DirectX::XMFLOAT4(0, pPosition.y, 0, 0), size);
}

void GameStick::ChangeState(Direction state)
{
    pState = state;
}

void GameStick::ChangeSize()
{
    size.y = size.y + 0.1f;
    DirectX::BoundingBox newboundingBox;
    newboundingBox.Center = { pPosition.x, pPosition.y, 0 };
    newboundingBox.Extents = { 0.01f, 0.1f * size.y,0 };
    boundingBox = newboundingBox;
}

void GameStick::ResetSize()
{
    DirectX::BoundingBox newboundingBox;
    newboundingBox.Center = { pPosition.x, pPosition.y, 0 };
    newboundingBox.Extents = { 0.01f, 0.1f,0 };
    boundingBox = newboundingBox;
	size.y = 1.0f;
}

DirectX::XMFLOAT2 GameStick::GetPosition()
{
    return pPosition;
}
