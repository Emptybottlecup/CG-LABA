#pragma once
#include "Game.h"
#include "GameBall.h"

class Pong : public Game
{
public:
	Pong();

	void InitializeGame();

	void Run();

	void ChangeConstantBufferPong(DirectX::XMFLOAT4 offset, DirectX::XMFLOAT4 size);

	~Pong();
private:
	GameStick* pPaddle1;
	GameStick* pPaddle2;
	GameBall* pBall;

	int Player_1 = 0;
	int Player_2 = 0;


	DirectX::BoundingBox LeftWall;
	DirectX::BoundingBox RightWall;
	DirectX::BoundingBox UpWall;
	DirectX::BoundingBox DownWall;

	ID3D11Buffer* pConstantBufferPong;
};