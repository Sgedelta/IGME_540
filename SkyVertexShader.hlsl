#include "ShaderHeaders.hlsli"


// Buffer
cbuffer ExternalData : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
}





// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixelSky main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixelSky output;

    //copy view matrix and set translation to 0 - skies do not move, no matter where we look at them from
    matrix viewNoTranslate = viewMatrix;
    viewNoTranslate._14 = 0;
    viewNoTranslate._24 = 0;
    viewNoTranslate._34 = 0;
    
    matrix viewProj = mul(projectionMatrix, viewNoTranslate);
    output.position = mul(viewProj, float4(input.localPosition, 1.0f));
    
    //make sure it's as far away as possible
    output.position.z = output.position.w;
    
    //set the direction - as if we were sampling from origin
    output.sampleDir = input.localPosition;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}