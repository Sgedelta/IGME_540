#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Vertex.h"

class Mesh
{
public:

	Mesh(Vertex vertexList[], int vertexCount, int indexList[], int indexCount);
	~Mesh();
	Mesh(const Mesh&) = delete; // Remove copy constructor
	Mesh& operator=(const Mesh&) = delete; // Remove copy-assignment operator

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetVertextCount();
	int GetIndexCount();

	void Draw();


private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	int indexCount;
	int vertexCount;


};

