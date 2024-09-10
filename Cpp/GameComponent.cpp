#include "../Header/GameComponent.h"

GameComponent::GameComponent(Game* GameObject) : pGame(GameObject)
{

}

bool GameComponent::CompileD3DShader(const char* filePath, const char* entry, const char* shaderModel, ID3DBlob** buffer)
{
	ID3DBlob* errorBuffer = 0;
	HRESULT result;

	int bufferSize = MultiByteToWideChar(CP_ACP, 0, filePath, -1, NULL, 0);
	WCHAR* wideFilePath = new WCHAR[bufferSize];
	MultiByteToWideChar(CP_ACP, 0, filePath, -1, wideFilePath, bufferSize);

	result = D3DCompileFromFile(wideFilePath, 0, 0, entry, shaderModel, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, buffer, &errorBuffer);

	if (FAILED(result))
	{
		if (errorBuffer != 0)
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}
		return false;
	}

	if (errorBuffer != 0)
		errorBuffer->Release();
	
	return true;
}

void GameComponent::Reload()
{

}

TriangleGameComponent::TriangleGameComponent(Game* GameObject, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices, DirectX::XMFLOAT4 offset) : GameComponent(GameObject), pPoints(points), pIndices(indices)
{
	for (int i = 0; i < pPoints.size(); i += 2)
	{
		pPoints[i].x += offset.x;
		pPoints[i].y += offset.y;
	}
}

void TriangleGameComponent::Initialize()
{
	ID3DBlob* VertexShaderByteCode = 0;

	bool compileResult = CompileD3DShader("Shaders.txt", "VSMain", "vs_5_0", &VertexShaderByteCode);

	if (compileResult == false)
	{
		MessageBox(0, L"Failed to compile solid green color vertex shader", 0, 0);
		return;
	}

	HRESULT result = pGame->GetDevice()->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &pVertexShader);

	if (FAILED(result))
	{
		if (VertexShaderByteCode)
			VertexShaderByteCode->Release();

		return;
	}

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0},

		D3D11_INPUT_ELEMENT_DESC {
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		16,
		D3D11_INPUT_PER_VERTEX_DATA,
		0}
	};

	unsigned int totalLayoutElements = ARRAYSIZE(inputElements);

	result = pGame->GetDevice()->CreateInputLayout(inputElements, totalLayoutElements, VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &pInputLayout);

	VertexShaderByteCode->Release();

	if (FAILED(result))
	{
		return;
	}


	ID3DBlob* PixelShaderByteCode = 0;

	compileResult = CompileD3DShader("Shaders.txt", "PSMain", "ps_5_0", &PixelShaderByteCode);

	if (compileResult == false)
	{
		MessageBox(0, L"Failed to compile solid green color pixel shader", 0, 0);
		return;
	}

	result = pGame->GetDevice()->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &pPixelShader);

	PixelShaderByteCode->Release();

	if (FAILED(result))
	{
		return;
	}

	D3D11_BUFFER_DESC vertexBufDesc;
	ZeroMemory(&vertexBufDesc, sizeof(vertexBufDesc));

	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(pPoints);

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pPoints.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = pGame->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &pVertices);

	if (FAILED(result))
	{
		return;
	}

	D3D11_BUFFER_DESC indexBufDesc;
	ZeroMemory(&indexBufDesc, sizeof(indexBufDesc));

	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(pIndices);

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pGame->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &pIndexBuffer);

	if (FAILED(result))
	{
		return;
	}
}

void TriangleGameComponent::Draw()
{
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	pGame->GetDeviceContext()->IASetInputLayout(pInputLayout);
	pGame->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertices, strides, offsets);
	pGame->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pGame->GetDeviceContext()->VSSetShader(pVertexShader, 0, 0);
	pGame->GetDeviceContext()->PSSetShader(pPixelShader, 0, 0);
	pGame->GetDeviceContext()->DrawIndexed(pIndices.size(), 0, 0);
}

void TriangleGameComponent::Update(float deltaTim)
{

}

void TriangleGameComponent::DestroyResources()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pVertices->Release();
	pIndexBuffer->Release();
	pInputLayout->Release();

	pVertexShader = 0;
	pPixelShader = 0;
	pVertices = 0;
	pIndexBuffer = 0;
	pInputLayout = 0;
}

TriangleGameComponent::~TriangleGameComponent()
{
	pGame = nullptr;
	DestroyResources();
}
