struct VSInput
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 posWS : SV_POSITION;
    float4 color : COLOR;
};

VSOutput mainVS(VSInput input)
{
    VSOutput output;
    output.posWS = float4(input.pos.xyz, 1.0f);
    output.color = float4(input.color.rgb, 1.0f);
    
    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    return input.color;
}
