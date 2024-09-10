Texture2D ObjTexture;
SamplerState ObjSamplerState;

cbuffer AllInfo : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;

    float4 DirectionLight;
    float4 LightColor;
    float4 ViewPos;
    float4 KaSpecPowKsX;

    float4x4 InverseTransposeWorldMatrix;
};

struct VS_Input
{
    float4 pos : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float4 WorldPos : TEXCOORD1;
};

PS_Input VS_Main(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input) 0;

    vsOut.pos = mul(vertex.pos, worldMatrix);
    vsOut.pos = mul(vsOut.pos, viewMatrix);
    vsOut.pos = mul(vsOut.pos, projMatrix);

	
    vsOut.TexCoord = vertex.TexCoord;
    vsOut.WorldPos = mul(vertex.pos, worldMatrix);
    vsOut.Normal = mul(float4(vertex.Normal.xyz, 0.0f), InverseTransposeWorldMatrix);

    return vsOut;
}

float4 PS_Main(PS_Input frag) : SV_TARGET
{
    float4 diffVal = ObjTexture.Sample(ObjSamplerState, float2(frag.TexCoord.x, frag.TexCoord.y));
    clip(diffVal.a - 0.01f);

    float3 kd = diffVal.xyz;
    float constant = 1.0f;
    float attentiation = 1.0f;

    float3 normal = normalize(frag.Normal.xyz);
    float4 viewPos = mul(float4(frag.WorldPos.xyz, 1.0f), viewMatrix);
    float3 viewDir = normalize(-viewPos.xyz);




    float3 lightDir = float3(0.0f, 0.0f, 0.0f);
 


    [branch]
    if (KaSpecPowKsX.w == 0)
    {
        lightDir = normalize(DirectionLight.xyz);
    }
    else
    {
        lightDir = DirectionLight.xyz - viewPos;
        lightDir = normalize(lightDir);
        float distance = length(lightDir);
        float liner  = 0.09f;
        float quadratic = 0.032f;
        attentiation = 1.0f / (constant + liner * distance
        +quadratic * (distance * distance));
    }

  


    float diff = max(0, dot(frag.Normal, lightDir)) * attentiation;
    float4 diffuse = diff * float4(LightColor.xyz, 1.0f);


    float4 ambient = KaSpecPowKsX.x * float4(LightColor.xyz, 1.0f) * attentiation;

    float3 refVec = reflect(-DirectionLight, normal);
    float spec = pow(max(0, dot(viewDir, refVec)), KaSpecPowKsX.z) * attentiation;
    float4 specular = diffVal.w * spec * float4(LightColor.xyz, 1.0f);

    float4 col = ((diffuse + specular) + ambient) * diffVal;
    col.rgb = pow(col.rgb, 1.0f / 2.2f);
    return col;
}