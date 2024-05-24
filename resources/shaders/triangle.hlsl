#include "globals.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 posWS : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

VSOutput mainVS(VSInput input)
{
    VSOutput output;
    output.posWS = mul(g_perFrame.transformMat, float4(input.pos.xyz, 1.0f));
    output.normal = input.normal;
    output.uv = input.uv;
    
    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    //return float4(input.posWS.xyz, 1.0f);
    return float4(abs(input.normal.xyz), 1.0f);
}
