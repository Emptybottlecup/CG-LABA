#include "../Header/Mesh.h"

struct VertexPos
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Normal;
};

Mesh::Mesh(Game* GameInstance, std::vector<VertexPos>& vertices, std::vector<DWORD>& indices, std::wstring texturePath) : GameComponent(GameInstance), pVertices(vertices), pIndices(indices), pTexturePath(texturePath)
{
	
}

void Mesh::Initialize()
{
	HRESULT d3dResult;

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * pVertices.size();
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = pVertices.data();
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
	indexDesc.ByteWidth = sizeof(DWORD) * pIndices.size();
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = pIndices.data();
	d3dResult = pGame->GetDevice()->CreateBuffer(&indexDesc,
		&resourceData, &pIndexBuffer);
	if (FAILED(d3dResult))
	{
		OutputDebugString(L"Failed to create index buffer\n");
		return;
	}

	if(pTexturePath != L"")
	{
		DirectX::CreateWICTextureFromFile(pGame->GetDevice(), pTexturePath.c_str(), nullptr, &pTextureRV);
	}

}

void Mesh::Update(float deltaTime)
{

}

void Mesh::Draw()
{
	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pGame->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pGame->GetDeviceContext()->PSSetShaderResources(0, 1, &pTextureRV);
	pGame->GetDeviceContext()->DrawIndexed(pIndices.size(), 0, 0);
}

void Mesh::DestroyResources()
{
	pVertexBuffer->Release();
	pIndexBuffer->Release();

	pVertexBuffer = 0;
	pIndexBuffer = 0;
}

Mesh::~Mesh()
{
	DestroyResources();
}
