#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, int materialType, float roughness)
{
	this->colorTint = colorTint;
	simpleVertexShader = vs;
	simplePixelShader = ps;

	this->materialType = materialType;
	this->roughness = roughness;

	uvScale = DirectX::XMFLOAT2(1, 1);
	uvOffset = DirectX::XMFLOAT2(0, 0);
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

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}

void Material::SetUVScale(DirectX::XMFLOAT2 newScale)
{
	uvScale = newScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 newOffset)
{
	uvOffset = newOffset;
}

int Material::GetTextureCount()
{
	return textureSRVs.size();
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetTextureSRVs()
{
	return textureSRVs;
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

int Material::GetMaterialType()
{
	return materialType;
}

void Material::SetMaterialType(int newType)
{
	materialType = newType;
}

void Material::AddTextureSRV(std::string textureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV)
{
	textureSRVs.insert({textureName, textureSRV});
}

void Material::AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ samplerName, sampler });
}

void Material::PrepareMaterial(Camera* cameraPtr)
{
	//bind
	BindMaterialShaders();

	//set texture stuff
	for (auto& t : textureSRVs) { simplePixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { simplePixelShader->SetSamplerState(s.first.c_str(), s.second); }

	//send some data to the shader
	simplePixelShader->SetFloat2("uvScale", uvScale);
	simplePixelShader->SetFloat2("uvOffset", uvOffset);
	simplePixelShader->SetFloat3("cameraPos", cameraPtr->GetTransform()->GetPosition());
	simplePixelShader->SetFloat("roughness", roughness);
}

void Material::BindMaterialShaders()
{
	simpleVertexShader->SetShader();
	simplePixelShader->SetShader();
}
