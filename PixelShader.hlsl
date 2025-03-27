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
    float3 ambient;
    Light lights[MAX_LIGHT_COUNT];
    int lightCount;
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
    input.normal = normalize(input.normal);
    
    float specExp = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float3 V = normalize(cameraPos - input.worldPosition);

    float4 sampleColor = SurfaceTexture.Sample(BasicSampler, input.uv * uvScale + uvOffset);

    float3 c = float3(0, 0, 0);
    for (int i = 0; i < lightCount; ++i) {
        switch (lights[i].Type) {
        case LIGHT_TYPE_DIRECTIONAL:
            float3 specularColor = 0;
            if (specExp != 0) { 
                specularColor = SpecularHighlight(lights[i], specExp, input.normal, V);
            }
            c += DiffuseColor(lights[i], input.normal, sampleColor * colorTint) + specularColor;
            break;
        case LIGHT_TYPE_POINT:

            break;
        case LIGHT_TYPE_SPOT:

            break;
        }
    }

    return float4(c + ambient * sampleColor, 1);

    return float4(colorTint.x * sampleColor.x * ambient.x, colorTint.y * sampleColor.y * ambient.y, colorTint.z * sampleColor.z * ambient.z, colorTint.w * sampleColor.w);
}