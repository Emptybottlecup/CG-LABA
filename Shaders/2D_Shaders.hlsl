struct vertexIn
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct pixelIn
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
       float4 offset;
       float4 changesize;
};

pixelIn VSMainMove(vertexIn input)
{
    pixelIn output;
    output.position = input.position + offset;
    output.position.y = output.position.y * changesize.y;
    output.color = input.color;
	return output;
}


pixelIn VSMain(vertexIn input)
{
    pixelIn output;
    output.position = input.position;
    output.color = input.color;
	return output;
}

float4 PSMain(pixelIn input) : SV_TARGET 
{
    float4 color = input.color;
    return color;
}