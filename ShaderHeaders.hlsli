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

float3 DirectionToLight(Light light, float3 worldPos)
{
    switch (light.Type)
    {
        case LIGHT_TYPE_DIRECTIONAL:
            return normalize(-light.Direction);
            break;
        
        case LIGHT_TYPE_POINT:
        case LIGHT_TYPE_SPOT:
            return normalize(light.Position - worldPos);
            break;
    }
    return float3(0, 0, 0);
}

float3 DiffuseColor(Light light, float3 normal, float3 surfaceTint, float3 samplePos)
{
    
    return saturate(dot(normal, DirectionToLight(light, samplePos))) * light.Color * surfaceTint * light.Intensity;

}

float3 DiffuseColor(Light light, float3 normal, float3 surfaceTint)
{
    
    return DiffuseColor(light, normal, surfaceTint, float3(0, 0, 0));

}

float3 SpecularHighlight(Light light, float specExp, float3 normal, float3 V)
{
    float3 refl = normalize(reflect(light.Direction, normal));

    
    
    return pow(max(dot(refl, V), 0.0f), specExp);

}

float PointAttenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float SpotAttenuate(Light light, float3 worldPos)
{
    float pixelAngle = saturate(dot(worldPos - light.Position, light.Direction));
    
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    return spotTerm * PointAttenuate(light, worldPos);
}

float3 DirectionalLight(Light light, float3 normal, float3 surface, float3 V, float specExp)
{
    float3 specularColor = float3(0, 0, 0);
    if (specExp != 0)
    {
        specularColor = SpecularHighlight(light, specExp, normal, V);

    }
    return DiffuseColor(light, normal, surface) + specularColor;
}

float3 PointLight(Light light, float3 normal, float3 surface, float3 V, float3 samplePos, float specExp)
{
    float3 specularColor = float3(0, 0, 0);
    if (specExp != 0)
    {
        specularColor = SpecularHighlight(light, specExp, normal, V);

    }
    return (DiffuseColor(light, normal, surface, samplePos) + specularColor) * PointAttenuate(light, samplePos);

}

float3 SpotLight(Light light, float3 normal, float3 surface, float3 V, float3 samplePos, float specExp)
{
    float3 specularColor = float3(0, 0, 0);
    if (specExp != 0)
    {
        specularColor = SpecularHighlight(light, specExp, normal, V);

    }
    return (DiffuseColor(light, normal, surface, samplePos) + specularColor) * SpotAttenuate(light, samplePos);
}


#endif