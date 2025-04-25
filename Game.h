#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:


	std::vector<std::shared_ptr<Mesh>> meshPtrs;
	std::vector<std::shared_ptr<Entity>> entityPtrs;
	std::vector<std::shared_ptr<Camera>> cameraPtrs;
	int cameraIndex = 0;


	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateShaderToEntity();
	void CreateCameras();
	void UpdateImGui(float deltaTime);
	void BuildUI();
	void CreateShadowmapResources();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	std::vector<std::shared_ptr<Material>> materials;
	std::shared_ptr<Sky> sky;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	std::shared_ptr<SimpleVertexShader> shadowVS;
	std::vector<DirectX::XMFLOAT4X4> lightViewMatrixList;
	std::vector<DirectX::XMFLOAT4X4> lightProjectionMatrixList;
	int shadowMapResolution = 2048; //Ideally a power of 2
	float lightProjectionSize = 50.0f;

	//ImGui data
	float ImGui_bgColor[4];
	float ImGui_colorTint[4];
	float ImGui_offset[3];
	bool ImGui_Demo_Show = false;
	std::vector<float> entityData;
	std::vector<Light> lights;
	DirectX::XMFLOAT3 ambientColor;
};

