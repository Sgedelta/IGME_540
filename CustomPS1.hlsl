
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition : SV_POSITION;	// XYZW position (System Value Position)
	float2 uv			  : TEXTCOORD;
    float3 normal		  : NORMAL;
};

// Buffer
cbuffer ExternalData : register(b0)
{
	float timeInSeconds;
	
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
	//generate a float rgb for pattern
    float3 patternInput = float3(sin(input.screenPosition.x * input.screenPosition.y * 75), cos(input.screenPosition.y * input.screenPosition.x * 75), tan(input.normal.x + input.normal.y + input.normal.z));

	//interpreted from glsl into hlsl, rgb2hsv and hsv2rgb
	// https://web.archive.org/web/20200207113336/http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(patternInput.bg, K.wz), float4(patternInput.gb, K.xy), step(patternInput.b, patternInput.g));
    float4 q = lerp(float4(p.xyw, patternInput.r), float4(patternInput.r, p.yzx), step(p.x, patternInput.r));
	
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
	
    float3 hsv = float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
	
	//now, change the hue value based on time
    hsv.x += (timeInSeconds + sin(input.screenPosition.x / 100)) / 5; //update!
	
	//return back to rgb
    float4 K2 = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p2 = abs(frac(hsv.xxx + K2.xyz) * 6.0 - K2.www);
    float3 finalRGB = hsv.z * lerp(K2.xxx, saturate(p2 - K2.xxx), hsv.y);
	
	//return float4(
		
	//1);

    return float4(finalRGB, 1);
}