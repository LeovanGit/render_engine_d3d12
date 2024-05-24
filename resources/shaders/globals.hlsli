struct PerFrame
{
    float4x4 transformMat;
};

ConstantBuffer<PerFrame> g_perFrame : register(b0);