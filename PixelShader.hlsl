#include "ShaderHeaders.hlsli"

//texture and sampler buffers
Texture2D Albedo : register(t0); // "t" registers for textures
Texture2D NormalTexture : register(t1); //the normal map for our texture
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
Texture2D ShadowMap : register(t4);

SamplerState BasicSampler : register(s0); // "s" registers for samplers
SamplerComparisonState ShadowSampler : register(s1);


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
    input.shadowMapPos /= input.shadowMapPos.w;
    //convert to UVs for sample
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; //flip y
    //distance to light
    float distToLight = input.shadowMapPos.z;
    float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, distToLight/*-.015f Subtracting this "raises" the floor that counts as "1"*/).r;
    
    return float4(shadowAmount, distToLight, 0, 1);
    
    float2 adjustedUv = input.uv * uvScale + uvOffset;
    
    //unpack normal:
    float3 unpackedNormal = NormalTexture.Sample(BasicSampler, adjustedUv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    
    float3 N = normalize(input.normal); // normalized here (or before, if needed) because it may have become unnormalized before
    float3 T = normalize(input.tangent); // as above
    T = normalize(T - N * dot(T, N)); // ENSURING normalization and orthogonalness
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N); // A Rotational matrix that is local space for the pixel
    
    input.normal = mul(unpackedNormal, TBN); // update the normal we actually use later to be the unpacked normal and the local space
    
    float specExp = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float3 V = normalize(cameraPos - input.worldPosition);

    float4 sampleColor = pow(Albedo.Sample(BasicSampler, adjustedUv), 2.2f);
    float roughness = RoughnessMap.Sample(BasicSampler, adjustedUv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, adjustedUv).r;
    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL, sampleColor.rgb, metalness); 

    float3 c = float3(0, 0, 0);

    for (int i = 0; i < lightCount; ++i) {
        switch (lights[i].Type) {
        case LIGHT_TYPE_DIRECTIONAL:
                if (i == 0)
                {
                    c += DirectionalLight(lights[i], input.normal, (float3) sampleColor, V, specularColor, roughness, metalness) * shadowAmount;
                }
                else
                {
                    c += DirectionalLight(lights[i], input.normal, (float3) sampleColor, V, specularColor, roughness, metalness);
                }

            break;
        case LIGHT_TYPE_POINT:
                c += PointLight(lights[i], input.normal, (float3) sampleColor, V, input.worldPosition, specularColor, roughness, metalness);
            break;
        case LIGHT_TYPE_SPOT:
                c += SpotLight(lights[i], input.normal, (float3) sampleColor, V, input.worldPosition, specularColor, roughness, metalness);
            break;
        } 

    }
    
    float3 totalColor = c + (float3) sampleColor;

    return float4(pow(totalColor, 1.0f / 2.2f), 1);

}