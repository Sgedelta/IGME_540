#include "ShaderHeaders.hlsli"



// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	float2 uv				: TEXTCOORD;    
	float3 normal			: NORMAL;
    float3 tangent			: TANGENT;
};

// Buffer
cbuffer ExternalData : register(b0)
{
    matrix worldMatrix;
    float2 uv : TEXTCOORD;
    float3 normal : NORMAL;
    matrix viewMatrix;
    matrix projectionMatrix;
	matrix worldInvTranspose;
	
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



    output.normal = mul((float3x3)worldInvTranspose, input.normal);
	output.worldPosition = mul(worldMatrix, float4(input.localPosition, 1)).xyz;

	output.uv = input.uv;
    output.tangent = mul((float3x3) worldMatrix, input.tangent);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}