#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Vertex.h"

class Mesh
{
public:

	Mesh(Vertex vertexList[], int vertexCount, UINT indexList[], int indexCount);
	Mesh(const char* objFile);
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

	void CreateBuffers(Vertex vertexList[], int vertexCount, UINT indexList[], int indexCount);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	int indexCount;
	int vertexCount;


};

