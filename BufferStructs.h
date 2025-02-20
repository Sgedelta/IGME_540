#pragma once


#include <directxmath.h>

struct VertexShaderData {
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4 tint;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

};