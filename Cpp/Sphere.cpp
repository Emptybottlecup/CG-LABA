#include "../Header/Sphere.h"

#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct VertexPos
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Normal;
};

Sphere::Sphere(Game* GameInstance, int radius, int latitudeSegments, int longitudeSegments, Sphere* parent, const DirectX::XMFLOAT3& position, const int speed) : GameComponent(GameInstance), pRadius(radius), pLatitudeSegments(latitudeSegments), pLongitudeSegments(longitudeSegments), pParent(parent), pPosition(position), pSpeed(speed)
{
	pScale = { 1.0f, 1.0f, 1.0f };
	pRotation = { 0.0f, 0.0f, 0.0f };
	pQuaternion = DirectX::XMQuaternionIdentity();
	pDistanceFromParent = DirectX::XMVectorZero();
}

Sphere::Sphere(Game* GameInstance, int radius, int latitudeSegments, int longitudeSegments, Sphere* parent, const DirectX::XMFLOAT3& position, const int speed, const wchar_t* filename) : GameComponent(GameInstance), pRadius(radius), pLatitudeSegments(latitudeSegments), pLongitudeSegments(longitudeSegments), pParent(parent), pPosition(position), pSpeed(speed), pTextureFilename(filename)
{
	pScale = { 1.0f, 1.0f, 1.0f };
	pRotation = { 0.0f, 0.0f, 0.0f };
	pQuaternion = DirectX::XMQuaternionIdentity();
	pDistanceFromParent = DirectX::XMVectorZero();
}

float Sphere::GetSpeed()
{
	return pSpeed;
}

void Sphere::Initialize()
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

	if (pParent)
	{
		DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&pPosition);
		DirectX::XMVECTOR parentpos = DirectX::XMLoadFloat3(pParent->GetPosition());
		pDistanceFromParent = DirectX::XMVectorSubtract(positionVector, parentpos);
		pSpeed += pParent->GetSpeed();
	}
}

void Sphere::Update(float deltaTime)
{
	pRotation.y += 1.0f * deltaTime;
	if (pParent)
	{
		pAngle += 1.0f * deltaTime * pSpeed;
		DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		pQuaternion = DirectX::XMQuaternionRotationAxis(rotationAxis, DirectX::XMConvertToRadians(pAngle));
		DirectX::XMVECTOR parentPosition = DirectX::XMLoadFloat3(pParent->GetPosition());
		DirectX::XMVECTOR positionVector = DirectX::XMVectorAdd(DirectX::XMVector3Rotate(pDistanceFromParent, pQuaternion), parentPosition);
		DirectX::XMStoreFloat3(&pPosition, positionVector);
	}
}

DirectX::XMFLOAT3* Sphere::GetPosition()
{
	return &pPosition;
}

void Sphere::Draw()
{
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
		DirectX::XMMatrixRotationRollPitchYaw(pRotation.x, pRotation.y, pRotation.z) * DirectX::XMMatrixTranslation(pPosition.x, pPosition.y, pPosition.z);
	pGame->ChangeConstantBuffer(WorldMatrix, pGame->GetCamera()->GetViewMatrix(), pGame->GetCamera()->GetProjectionMatrix(), DirectX::XMMatrixIdentity());
	pGame->GetDeviceContext()->DrawIndexed(pIndexCount, 0, 0);
}

void Sphere::DestroyResources()
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

Sphere::~Sphere()
{
	DestroyResources();
}