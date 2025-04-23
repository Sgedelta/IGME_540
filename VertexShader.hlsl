#include "ShaderHeaders.hlsli"



// Buffer
cbuffer ExternalData : register(b0)
{
    matrix worldMatrix;
    float2 uv : TEXTCOORD;
    float3 normal : NORMAL;
    matrix viewMatrix;
    matrix projectionMatrix;
	matrix worldInvTranspose;
    matrix lightView;
    matrix lightProjection;
	
}



// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

    matrix wvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

    matrix shadowWVP = mul(lightProjection, mul(lightView, worldMatrix));
    output.shadowMapPos = mul(shadowWVP, float4(input.localPosition, 1.0f));


    output.normal = mul((float3x3)worldInvTranspose, input.normal);
	output.worldPosition = mul(worldMatrix, float4(input.localPosition, 1)).xyz;

	output.uv = input.uv;
    output.tangent = mul((float3x3) worldMatrix, input.tangent);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}