#pragma once
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>
#include "SimpleShader.h"

class Material
{

public:

	Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, int materialType);
	~Material();
	Material(const Material&) = delete; // Remove copy constructor
	Material& operator=(const	Material&) = delete; // Remove copy-assignment operator

	DirectX::XMFLOAT4 GetColorTint();
	void SetColorTint(DirectX::XMFLOAT4 newTint);
	DirectX::XMFLOAT2 GetUVScale();
	void SetUVScale(DirectX::XMFLOAT2 newScale);
	DirectX::XMFLOAT2 GetUVOffset();
	void SetUVOffset(DirectX::XMFLOAT2 newOffset);
	int GetTextureCount();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureSRVs();


	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vs);
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	void SetPixelShader(std::shared_ptr<SimplePixelShader> ps);

	int GetMaterialType();
	void SetMaterialType(int newType);

	void AddTextureSRV(std::string textureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

	void PrepareMaterial();


private:

	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	std::shared_ptr<SimpleVertexShader> simpleVertexShader;
	std::shared_ptr<SimplePixelShader> simplePixelShader;
	int materialType;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

};

