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
    int blurAmount;
    float pixelSize;
	
}


float4 main(VertexToPixel input) : SV_TARGET
{
    float4 pixelColor = 0;
    int sampleCount = 0;
    
    for (int i = -blurAmount; i <= blurAmount; ++i)
    {
        float2 uv = input.uv;
        uv += float2(i * pixelSize, 0);
        pixelColor += Pixels.Sample(ClampSampler, uv);
        sampleCount++;
    }
    pixelColor /= sampleCount;

    return pixelColor;
}