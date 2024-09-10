#pragma once
#include "BoxGameComponent.h"

enum class Direction
{
	UP,
	DOWN,
	STOP
};

class GameStick : public BoxGameComponent
{
public:
	GameStick(Game* GameObject, const DirectX::XMFLOAT2& position, Pong* pong);

	void Update(float deltaTime);

	void ChangeState(Direction state);

	void ChangeSize();

	void ResetSize();
	DirectX::XMFLOAT2 GetPosition();
private:
	Direction pState = Direction::STOP;
	DirectX::XMFLOAT2 pPosition;
	DirectX::XMFLOAT4 size = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	float pSpeed = 0.1;
};
