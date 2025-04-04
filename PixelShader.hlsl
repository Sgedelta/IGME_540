#include "ShaderHeaders.hlsli"

//texture and sampler buffers
Texture2D SurfaceTexture : register(t0); // "t" registers for textures
Texture2D NormalTexture : register(t1);

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
    
    //unpack normal:
    float3 unpackedNormal = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    
    float3 N = normalize(input.normal); // normalized here (or before, if needed) because it may have become unnormalized before
    float3 T = normalize(input.tangent); // as above
    T = normalize(T - N * dot(T, N)); // ENSURING normalization and orthogonalness
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N); // A Rotational matrix that is local space for the pixel
    
    input.normal = mul(unpackedNormal, TBN); // update the normal we actually use later to be the unpacked normal and the local space
    
    
    float specExp = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float3 V = normalize(cameraPos - input.worldPosition);

    float4 sampleColor = SurfaceTexture.Sample(BasicSampler, input.uv * uvScale + uvOffset);

    float3 c = float3(0, 0, 0);

    for (int i = 0; i < lightCount; ++i) {
        switch (lights[i].Type) {
        case LIGHT_TYPE_DIRECTIONAL:
                c += DirectionalLight(lights[i], input.normal, (float3)sampleColor, V, specExp);
            break;
        case LIGHT_TYPE_POINT:
                c += PointLight(lights[i], input.normal, (float3)sampleColor, V, input.worldPosition, specExp);
            break;
        case LIGHT_TYPE_SPOT:
                c += SpotLight(lights[i], input.normal, (float3) sampleColor, V, input.worldPosition, specExp);
            break;
        }

    }

    return float4(c + ambient * (float3) sampleColor, 1);

    return float4(colorTint.x * sampleColor.x * ambient.x, colorTint.y * sampleColor.y * ambient.y, colorTint.z * sampleColor.z * ambient.z, colorTint.w * sampleColor.w);
}