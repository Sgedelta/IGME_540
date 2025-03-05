#pragma once
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"

class Material
{

public:

	Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps);
	~Material();
	Material(const Material&) = delete; // Remove copy constructor
	Material& operator=(const	Material&) = delete; // Remove copy-assignment operator

	DirectX::XMFLOAT4 GetColorTint();
	void SetColorTint(DirectX::XMFLOAT4 newTint);
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vs);
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	void SetPixelShader(std::shared_ptr<SimplePixelShader> ps);

private:

	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> simpleVertexShader;
	std::shared_ptr<SimplePixelShader> simplePixelShader;

};

