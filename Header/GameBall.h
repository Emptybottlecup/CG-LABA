#pragma once
#include "BoxGameComponent.h"

class GameBall : public BoxGameComponent
{
public:
	GameBall(Game* GameObject, const DirectX::XMFLOAT2& position, Pong* pong);

	void Update(float deltaTime);

	void ChangeDirection(float PaddleCenterY, float PaddleHeight, bool LeftRight);

	void Reload() override;
	void ChangeUpDown();

	~GameBall();
private:
	DirectX::XMFLOAT2 direction = { -1.0f, 0.0f };
	DirectX::XMFLOAT2 pPosition;
	float pSpeed = 0.1;
};
