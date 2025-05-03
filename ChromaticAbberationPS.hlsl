struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};
Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);


// Buffer
cbuffer ExternalData : register(b0)
{
    float3 offsets;
    int mode;
    float2 mousePos;
	
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 pixelColor = 0;
    float2 uv = input.uv;
    float2 dir = 0;
    
    switch (mode)
    {
        case 0: //horizontal
            dir = float2(1, 0);
            break;
        case 1: //vertical
            dir = float2(0, 1);
            break;
        case 2: //radial
            dir = normalize(uv - mousePos);
            break;
    }
    pixelColor.r = Pixels.Sample(ClampSampler, uv + (dir * offsets.r)).r;
    pixelColor.g = Pixels.Sample(ClampSampler, uv + (dir * offsets.g)).g;
    pixelColor.b = Pixels.Sample(ClampSampler, uv + (dir * offsets.b)).b;
    pixelColor.a = Pixels.Sample(ClampSampler, uv).a;
    
    
    return pixelColor;
}