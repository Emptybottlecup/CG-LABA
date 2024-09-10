#include "../Header/CharacterBall.h"
#include <cmath>
#include <iostream>
#include "../Header/Camera.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct VertexPos
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Normal;
};

CharacterBall::CharacterBall(Game* GameInstance, int radius, int latitudeSegments, int longitudeSegments, const wchar_t* filename, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation) : GameComponent(GameInstance), pRadius(radius), pScale(scale), pRotation(rotation),
	pLatitudeSegments(latitudeSegments), pLongitudeSegments(longitudeSegments), pTextureFilename(filename), pPosition(position)
{
	pQuaternion = DirectX::XMQuaternionIdentity();

	pCollision.Radius = radius;
	pCollision.Center = position;
}

void CharacterBall::Initialize()
{
	HRESULT d3dResult;

	ID3DBlob* vsBuffer = 0;
	bool compileResult = CompileD3DShader("Shaders/3D_Shaders.hlsl", "VS_Main", "vs_5_0", &vsBuffer);
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
	compileResult = CompileD3DShader("Shaders/3D_Shaders.hlsl", "PS_Main", "ps_5_0", &psBuffer);
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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);
	d3dResult = pGame->GetDevice()->CreateInputLayout(solidColorLayout, totalLayoutElements, vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), &pInputLayout);
	vsBuffer->Release();
	if (FAILED(d3dResult))
	{
		return;
	}

	std::vector<VertexPos> vertices;
	for (int lat = 0; lat <= pLatitudeSegments; ++lat) {
		float phi = M_PI * float(lat) / float(pLatitudeSegments);
		float v = float(lat) / float(pLatitudeSegments);

		for (int lon = 0; lon <= pLongitudeSegments; ++lon) {
			float theta = 2 * M_PI * float(lon) / float(pLongitudeSegments);
			float u = float(lon) / float(pLongitudeSegments);

			DirectX::XMFLOAT3 vertexPos;
			vertexPos.x = pRadius * sinf(phi) * cosf(theta);
			vertexPos.y = pRadius * cosf(phi);
			vertexPos.z = pRadius * sinf(phi) * sinf(theta);

			// Calculate the normal as the normalized position vector
			DirectX::XMFLOAT3 normal;
			normal.x = sinf(phi) * cosf(theta);
			normal.y = cosf(phi);
			normal.z = sinf(phi) * sinf(theta);

			// Normalize the normal vector
			DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&normal);
			normalVec = DirectX::XMVector3Normalize(normalVec);
			DirectX::XMStoreFloat3(&normal, normalVec);

			VertexPos vertex({ vertexPos, {u, v}, normal });
			vertices.push_back(vertex);
		}
	}

	std::vector<int> indices;
	for (int lat = 0; lat < pLatitudeSegments; ++lat) {
		for (int lon = 0; lon < pLongitudeSegments; ++lon) {
			unsigned int first = (lat * (pLongitudeSegments + 1)) + lon;
			unsigned int second = first + pLongitudeSegments + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * vertices.size();
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices.data();
	d3dResult = pGame->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, &pVertexBuffer);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create vertex buffer\n");
		return;
	}

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(indexDesc));
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(int) * indices.size();
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = indices.data();
	d3dResult = pGame->GetDevice()->CreateBuffer(&indexDesc,
		&resourceData, &pIndexBuffer);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create index buffer\n");
		return;
	}

	pIndexCount = indices.size();

	d3dResult = DirectX::CreateWICTextureFromFile(pGame->GetDevice(), pTextureFilename, nullptr, &pTextureRV);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create texture\n");
		return;
	}

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	d3dResult = pGame->GetDevice()->CreateSamplerState(&sampDesc, &pSamplerLinear);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create sampler\n");
	}
}

void CharacterBall::Update(float deltaTime)
{
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    DirectX::XMVECTOR Front = pCamera->GetFrontVector();
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);
    front.y = 0;
    Front = DirectX::XMLoadFloat3(&front);

    DirectX::XMVECTOR Right = pCamera->GetRightVector();
    DirectX::XMFLOAT3 right;
    DirectX::XMStoreFloat3(&right, Right);
    right.y = 0;
    Right = DirectX::XMLoadFloat3(&right);

    Front = DirectX::XMVector3Normalize(Front);
    Right = DirectX::XMVector3Normalize(Right);

    if (pGame->GetInputDevice()->IsKeyDown(Keys::W))
    {
        direction = DirectX::XMVectorAdd(direction, Front);
    }
    if (pGame->GetInputDevice()->IsKeyDown(Keys::S))
    {
        direction = DirectX::XMVectorSubtract(direction, Front);
    }
    if (pGame->GetInputDevice()->IsKeyDown(Keys::A))
    {
        direction = DirectX::XMVectorAdd(direction, Right);
    }
    if (pGame->GetInputDevice()->IsKeyDown(Keys::D))
    {
        direction = DirectX::XMVectorSubtract(direction, Right);
    }

    DirectX::XMVECTOR pPositionVector = DirectX::XMLoadFloat3(&pPosition);
    direction = DirectX::XMVector3Normalize(direction);
    pPositionVector = DirectX::XMVectorAdd(pPositionVector, DirectX::XMVectorScale(direction, pCamera->GetCameraSpeed() * deltaTime));
    DirectX::XMStoreFloat3(&pPosition, pPositionVector);
	pCollision.Center = pPosition;


	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(direction, upVector);
	if (!DirectX::XMVector3IsInfinite(rotationAxis) && !DirectX::XMVector3Equal(rotationAxis, DirectX::XMVectorZero()))
	{
		rotationAxis = DirectX::XMVector3Normalize(rotationAxis);
		float distance = pCamera->GetCameraSpeed() * deltaTime;
		float rotationAngle = distance / pRadius * -1;
		pQuaternion = DirectX::XMQuaternionMultiply(pQuaternion, DirectX::XMQuaternionRotationAxis(rotationAxis, rotationAngle));
	}
}

DirectX::XMFLOAT3* CharacterBall::GetPosition()
{
	return &pPosition;
}

void CharacterBall::SetCamera(Camera* camera)
{
	pCamera = camera;
}

DirectX::BoundingSphere* CharacterBall::GetCollision()
{
	return &pCollision;
}

DirectX::XMVECTOR CharacterBall::GetQuaternion()
{
	return pQuaternion;
}

void CharacterBall::Draw()
{
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(pQuaternion);
	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;
	pGame->GetDeviceContext()->IASetInputLayout(pInputLayout);
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pGame->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pGame->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pGame->GetDeviceContext()->VSSetShader(pVertexShader, 0, 0);
	pGame->GetDeviceContext()->PSSetShader(pPixelShader, 0, 0);
	pGame->GetDeviceContext()->PSSetShaderResources(0, 1, &pTextureRV);
	pGame->GetDeviceContext()->PSSetSamplers(0, 1, &pSamplerLinear);
	DirectX::XMMATRIX WorldMatrix = DirectX::XMMatrixScaling(pScale.x, pScale.y, pScale.z) *
		rotationMatrix * DirectX::XMMatrixTranslation(pPosition.x, pPosition.y, pPosition.z);
	DirectX::XMMATRIX InvTrWorldView = DirectX::XMMatrixScaling(pScale.x, pScale.y, pScale.z) *
		rotationMatrix;
	InvTrWorldView = DirectX::XMMatrixInverse(0, InvTrWorldView);
	InvTrWorldView = DirectX::XMMatrixTranspose(InvTrWorldView);
	InvTrWorldView = InvTrWorldView * pGame->GetCamera()->GetViewMatrix();
	pGame->ChangeConstantBuffer(WorldMatrix, pGame->GetCamera()->GetViewMatrix(), pGame->GetCamera()->GetProjectionMatrix(), InvTrWorldView);
	pGame->GetDeviceContext()->DrawIndexed(pIndexCount, 0, 0);
}

void CharacterBall::DestroyResources()
{
	if (pInputLayout) pInputLayout->Release();
	if(pTextureRV) pTextureRV->Release();
	if(pSamplerLinear) pSamplerLinear->Release();
	if(pVertexShader) pVertexShader->Release();
	if(pPixelShader) pPixelShader->Release();
	if(pVertexBuffer) pVertexBuffer->Release();
	if (pIndexBuffer) pIndexBuffer->Release();

	pVertexShader = 0;
	pPixelShader = 0;
	pVertexBuffer = 0;
	pIndexBuffer = 0;
	pInputLayout = 0;
	pTextureRV = 0;
	pSamplerLinear = 0;
}

CharacterBall::~CharacterBall()
{
	DestroyResources();
}


/*
#include "CharacterBall.h"
#include <cmath>
#include <iostream>
#include "Camera.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct VertexPos
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Normal;
};

CharacterBall::CharacterBall(Game* GameInstance, const std::string& filenamel, const wchar_t* textureFilename, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, int radius) :
	Model(GameInstance, filenamel, textureFilename, position, scale, rotation)
{
	pCollision.Radius = radius;
	pCollision.Center = position;
}

void CharacterBall::Update(float deltaTime)
{
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR Front = pCamera->GetFrontVector();
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);
	front.y = 0;
	Front = DirectX::XMLoadFloat3(&front);

	DirectX::XMVECTOR Right = pCamera->GetRightVector();
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, Right);
	right.y = 0;
	Right = DirectX::XMLoadFloat3(&right);

	Front = DirectX::XMVector3Normalize(Front);
	Right = DirectX::XMVector3Normalize(Right);

	if (pGame->GetInputDevice()->IsKeyDown(Keys::W))
	{
		direction = DirectX::XMVectorAdd(direction, Front);
	}
	if (pGame->GetInputDevice()->IsKeyDown(Keys::S))
	{
		direction = DirectX::XMVectorSubtract(direction, Front);
	}
	if (pGame->GetInputDevice()->IsKeyDown(Keys::A))
	{
		direction = DirectX::XMVectorAdd(direction, Right);
	}
	if (pGame->GetInputDevice()->IsKeyDown(Keys::D))
	{
		direction = DirectX::XMVectorSubtract(direction, Right);
	}

	DirectX::XMVECTOR pPositionVector = DirectX::XMLoadFloat3(&pPosition);
	direction = DirectX::XMVector3Normalize(direction);
	pPositionVector = DirectX::XMVectorAdd(pPositionVector, DirectX::XMVectorScale(direction, pCamera->GetCameraSpeed() * deltaTime));
	DirectX::XMStoreFloat3(&pPosition, pPositionVector);
	pCollision.Center = pPosition;


	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(direction, upVector);
	if (!DirectX::XMVector3IsInfinite(rotationAxis) && !DirectX::XMVector3Equal(rotationAxis, DirectX::XMVectorZero()))
	{
		rotationAxis = DirectX::XMVector3Normalize(rotationAxis);
		float distance = pCamera->GetCameraSpeed() * deltaTime;
		float rotationAngle = distance / pRadius * -1;
		pQuaternion = DirectX::XMQuaternionMultiply(pQuaternion, DirectX::XMQuaternionRotationAxis(rotationAxis, rotationAngle));
	}
}

DirectX::XMFLOAT3* CharacterBall::GetPosition()
{
	return &pPosition;
}

void CharacterBall::SetCamera(Camera* camera)
{
	pCamera = camera;
}

DirectX::BoundingSphere* CharacterBall::GetCollision()
{
	return &pCollision;
}

DirectX::XMVECTOR CharacterBall::GetQuaternion()
{
	return pQuaternion;
}

CharacterBall::~CharacterBall()
{

}
*/