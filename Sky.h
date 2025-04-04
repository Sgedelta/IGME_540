#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>

#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"


class Sky
{
public:
	Sky
	(
		std::shared_ptr<Mesh> mesh, 
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps,
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back	
	);
	~Sky();
	Sky(const Sky&) = delete; // Remove copy constructor
	Sky& operator=(const Sky&) = delete; // Remove copy-assignment operator

	void Draw(Camera* cameraPtr);


private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOpts;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skyDepthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerOpts;

	std::shared_ptr<Mesh> skyGeo;
	std::shared_ptr<SimplePixelShader> skyPs;
	std::shared_ptr<SimpleVertexShader> skyVs;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

};

