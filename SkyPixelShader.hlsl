#include "ShaderHeaders.hlsli"

TextureCube SkyMap : register(t0); 
SamplerState BasicSampler : register(s0);


float4 main(VertexToPixelSky input) : SV_TARGET
{   
    return SkyMap.Sample(BasicSampler, input.sampleDir);
}