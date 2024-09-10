#include "../Header/GameBall.h"
#include "../Header/Pong.h"
#define M_PI 3.141592653589793238462643383279502884L

GameBall::GameBall(Game* GameObject, const DirectX::XMFLOAT2& position, Pong* pong) : BoxGameComponent(GameObject, pong), pPosition(position)
{
	std::vector<DirectX::XMFLOAT4> Points =
	{
		DirectX::XMFLOAT4(position.x - 0.025f,position.y + 0.025f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + 0.025f,position.y + 0.025f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x - 0.025f, position.y - 0.025f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + 0.025f, position.y - 0.025f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};

	pPoints = Points;

	boundingBox.Center = { pPosition.x, pPosition.y, 0 };
	boundingBox.Extents = { 0.025f, 0.025f,0 };

}

void GameBall::Update(float deltaTime)
{
	pPosition.x += direction.x * deltaTime * pSpeed;
	pPosition.y += direction.y * deltaTime * pSpeed;
	boundingBox.Center = { pPosition.x, pPosition.y, 0 };
	pPong->ChangeConstantBufferPong(DirectX::XMFLOAT4(pPosition.x , pPosition.y, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1));
}

void GameBall::ChangeDirection(float PaddleCenterY, float PaddleHeight,bool LeftRight)
{
	float impactPoint = pPosition.y - PaddleCenterY;

	float normalizedImpactPoint = impactPoint / (PaddleHeight / 2);

	float reflectionAngleRadians = normalizedImpactPoint * (50.0f * (M_PI / 180.0f));

	if (LeftRight)
	{
		direction.x = cos(reflectionAngleRadians);
	}
	else
	{
		direction.x = -cos(reflectionAngleRadians);
	}
	direction.y = sin(reflectionAngleRadians);
}

void GameBall::Reload()
{
	direction.y = 0;
	direction.x *= -1;
	pPosition.x = 0;
	pPosition.y = 0;
	boundingBox.Center = { pPosition.x, pPosition.y, 0 };
}

void GameBall::ChangeUpDown()
{
	direction.y *= -1;
}

GameBall::~GameBall()
{

}
