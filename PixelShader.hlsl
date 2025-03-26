#include "ShaderHeaders.hlsli"

//texture and sampler buffers
Texture2D SurfaceTexture : register(t0); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers


// Buffer
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPos;
    float roughness;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    float4 sampleColor = SurfaceTexture.Sample(BasicSampler, input.uv * uvScale + uvOffset);
    return float4(colorTint.x * sampleColor.x, colorTint.y * sampleColor.y, colorTint.z * sampleColor.z, colorTint.w * sampleColor.w);
}