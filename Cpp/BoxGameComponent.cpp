#include "../Header/BoxGameComponent.h"
#include "../Header/Pong.h"

BoxGameComponent::BoxGameComponent(Game* GameObject, Pong* pong) : GameComponent(GameObject), pPong(pong)
{

}

BoxGameComponent::BoxGameComponent(Game* GameObject, Pong* pong, std::vector<DirectX::XMFLOAT4> points) : GameComponent(GameObject), pPoints(points), pPong(pong)
{

}

void BoxGameComponent::Initialize()
{
	ID3DBlob* VertexShaderByteCode = 0;

	bool compileResult = CompileD3DShader("Shaders/2D_Shaders.hlsl", "VSMainMove", "vs_5_0", &VertexShaderByteCode);

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

	compileResult = CompileD3DShader("Shaders/2D_Shaders.hlsl", "PSMain", "ps_5_0", &PixelShaderByteCode);

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
}

void BoxGameComponent::Draw()
{
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	pGame->GetDeviceContext()->IASetInputLayout(pInputLayout);
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertices, strides, offsets);
	pGame->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pGame->GetDeviceContext()->VSSetShader(pVertexShader, 0, 0);
	pGame->GetDeviceContext()->PSSetShader(pPixelShader, 0, 0);
	pGame->GetDeviceContext()->Draw(4, 0);
}

void BoxGameComponent::Update(float deltaTime)
{

}

void BoxGameComponent::DestroyResources()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pVertices->Release();
	pInputLayout->Release();

	pVertexShader = 0;
	pPixelShader = 0;
	pVertices = 0;
	pInputLayout = 0;
}

BoxGameComponent::~BoxGameComponent()
{
	pGame = nullptr;
	DestroyResources();
}
