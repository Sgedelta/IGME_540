#ifndef __GGP_SHADER_HEADER_INCLUDE__
#define __GGP_SHADER_HEADER_INCLUDE__


#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f
#define MAX_LIGHT_COUNT 128


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
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 uv : TEXTCOORD;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
};

struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding; //purposefully padding out 16 Byte Boundary
};

float3 DirectionToLight(Light light)
{
    return normalize(-light.Direction);
}

float3 DiffuseColor(Light light, float3 normal, float3 surfaceTint)
{
    return saturate(dot(normal, DirectionToLight(light))) * light.Color * surfaceTint * light.Intensity;

}

float3 SpecularHighlight(Light light, float specExp, float3 normal, float3 V)
{
    float3 refl = normalize(reflect(light.Direction, normal));

    
    
    return pow(max(dot(refl, V), 0.0f), specExp);

}

float DirectionalLight(Light light, float3 normal, float3 surface, float3 V, float specExp)
{
    return DiffuseColor(light, normal, surface) + SpecularHighlight(light, specExp, normal, V);
}



#endif