#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps)
{
	this->colorTint = colorTint;
	simpleVertexShader = vs;
	simplePixelShader = ps;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

void Material::SetColorTint(DirectX::XMFLOAT4 newTint)
{
	colorTint = newTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return simpleVertexShader;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vs)
{
	simpleVertexShader = vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return simplePixelShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> ps)
{
	simplePixelShader = ps;
}
