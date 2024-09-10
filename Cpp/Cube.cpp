#include "../Header/Cube.h"
#include <iostream>

struct VertexPos
{
	DirectX::XMFLOAT3 Pos;
};

Cube::Cube(Game* GameInstance) : GameComponent(GameInstance)
{
	pScale = { 1.0f, 1.0f, 1.0f };
	pPosition = { 0.0f, 0.0f, 0.0f };
	pRotation = { 0.0f, 0.0f, 0.0f };
	pDistanceFromParent = DirectX::XMVectorZero();
	pQuaternion = DirectX::XMQuaternionIdentity();
}

Cube::Cube(Game* GameInstance, const DirectX::XMFLOAT3& Position) : GameComponent(GameInstance)
{
	pScale = { 1.0f, 1.0f, 1.0f };
	pPosition = Position;
	pRotation = { 0.0f, 0.0f, 0.0f };
	pDistanceFromParent = DirectX::XMVectorZero();
	pQuaternion = DirectX::XMQuaternionIdentity();
}

Cube::Cube(Game* GameInstance, Cube* Parent, const DirectX::XMFLOAT3& Position, const DirectX::XMFLOAT3& Scale, const DirectX::XMFLOAT3& Rotation, const float speed) : GameComponent(GameInstance), pParent(Parent), pPosition(Position), pScale(Scale), pRotation(Rotation), pSpeed(speed)
{

}


void Cube::Initialize()
{
	HRESULT d3dResult;

	ID3DBlob* vsBuffer = 0;
	bool compileResult = CompileD3DShader("3D_Shaders.hlsl", "VS_Main", "vs_5_0", &vsBuffer);
	if (compileResult == false)
	{
		MessageBox(0, L"Failed to compile Vertex 3D_Shaders", 0, 0);
		return;
	}
	d3dResult = pGame->GetDevice()->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &pVertexShader);
	if (FAILED(d3dResult))
	{
		if (vsBuffer)
			vsBuffer->Release();
		return;
	}

	ID3DBlob* psBuffer = 0;
	compileResult = CompileD3DShader("3D_Shaders.hlsl", "PS_Main", "ps_5_0", &psBuffer);
	if (compileResult == false)
	{
		MessageBox(0, L"Failed to compile Pixel 3D_Shaders", 0, 0);
		return;
	}
	d3dResult = pGame->GetDevice()->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &pPixelShader);
	psBuffer->Release();
	if (FAILED(d3dResult))
	{
		return;
	}

	D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);
	d3dResult = pGame->GetDevice()->CreateInputLayout(solidColorLayout, totalLayoutElements, vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), &pInputLayout);
	vsBuffer->Release();
	if (FAILED(d3dResult))
	{
		return;
	}

	VertexPos vertices[] =
	{
		DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),
		DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f),
		DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f),
		DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f),
		DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f),
		DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),
		DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f)
	};
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * 24;
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;
	d3dResult = pGame->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, &pVertexBuffer);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create vertex buffer\n");
		return;
	}

	WORD indices[] =
	{
		3, 1, 0, 2, 1, 3,
		6, 4, 5, 7, 4, 6,
		11, 9, 8, 10, 9, 11,
		14, 12, 13, 15, 12, 14,
		19, 17, 16, 18, 17, 19,
		22, 20, 21, 23, 20, 22
	};
	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(indexDesc));
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(WORD) * 36;
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = indices;
	d3dResult = pGame->GetDevice()->CreateBuffer(&indexDesc,
		&resourceData, &pIndexBuffer);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create index buffer\n");
		return;
	}

	if (pParent)
	{
		DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&pPosition);
		DirectX::XMVECTOR parentpos = DirectX::XMLoadFloat3(pParent->GetPosition());
		pDistanceFromParent = DirectX::XMVectorSubtract(positionVector, parentpos);
	}
}

DirectX::XMFLOAT3* Cube::GetPosition()
{
	return &pPosition;
}

void Cube::Update(float deltaTime)
{
	if (pParent)
	{
		pAngle += 1.0f * deltaTime * pSpeed;
		DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(rotationAxis, DirectX::XMConvertToRadians(pAngle));
		DirectX::XMVECTOR parentpos = DirectX::XMLoadFloat3(pParent->GetPosition());
		DirectX::XMVECTOR positionVector = DirectX::XMVectorAdd(DirectX::XMVector3Rotate(pDistanceFromParent, quaternion), parentpos);
		DirectX::XMStoreFloat3(&pPosition, positionVector);
	}
}

void Cube::Draw()
{
	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;
	pGame->GetDeviceContext()->IASetInputLayout(pInputLayout);
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pGame->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pGame->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pGame->GetDeviceContext()->VSSetShader(pVertexShader, 0, 0);
	pGame->GetDeviceContext()->PSSetShader(pPixelShader, 0, 0);
	DirectX::XMMATRIX WorldMatrix = DirectX::XMMatrixScaling(pScale.x, pScale.y, pScale.z) *
		DirectX::XMMatrixRotationRollPitchYaw(pRotation.x, pRotation.y, pRotation.z) * DirectX::XMMatrixTranslation(pPosition.x, pPosition.y, pPosition.z);

	//ÍÓÆÍÎ ÁÓÄÅÒ ÏÎÌÅÍßÒÜ DirectX::XMMATRIX InvTrWorldView = DirectX::XMMatrixScaling(pScale.x, pScale.y, pScale.z) * rotationMatrix; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	DirectX::XMMATRIX InvTrWorldView = DirectX::XMMatrixScaling(pScale.x, pScale.y, pScale.z);
	InvTrWorldView = DirectX::XMMatrixInverse(0, InvTrWorldView);
	InvTrWorldView = DirectX::XMMatrixTranspose(InvTrWorldView);
	InvTrWorldView = InvTrWorldView * pGame->GetCamera()->GetViewMatrix();
	pGame->ChangeConstantBuffer(WorldMatrix, pGame->GetCamera()->GetViewMatrix(), pGame->GetCamera()->GetProjectionMatrix(), InvTrWorldView);


	pGame->GetDeviceContext()->DrawIndexed(36, 0, 0);
}

void Cube::DestroyResources()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pVertexBuffer->Release();
	pIndexBuffer->Release();
	pInputLayout->Release();

	pVertexShader = 0;
	pPixelShader = 0;
	pVertexBuffer = 0;
	pIndexBuffer = 0;
	pInputLayout = 0;
}

Cube::~Cube()
{
	DestroyResources();
}
