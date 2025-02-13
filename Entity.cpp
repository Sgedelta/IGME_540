#include "Entity.h"

#include <memory>
#include <wrl/client.h>

#include "Transform.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Graphics.h"

using namespace DirectX;

Entity::Entity(std::shared_ptr<Mesh> meshPtr)
{
	sharedMesh = meshPtr;
	sharedTransform = std::make_shared<Transform>();
}

Entity::~Entity()
{
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return sharedMesh;
}

std::shared_ptr<Transform> Entity::GetTransform()
{
	return sharedTransform;
}

void Entity::Draw(ID3D11Buffer *vertexConstBuffer, float tint[4])
{
	SendGPUData(vertexConstBuffer, tint);
	sharedMesh.get()->Draw();
}

void Entity::SendGPUData(ID3D11Buffer *vertexConstBuffer, float tint[4])
{
	//Vertex Shader Data:
	VertexShaderData vsData;
	vsData.tint = XMFLOAT4(tint[0], tint[1], tint[2], tint[3]);
	vsData.worldMatrix = sharedTransform->GetWorldMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(vertexConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer); //overwrite and "freeze" GPU

	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData)); // populate actual data

	Graphics::Context->Unmap(vertexConstBuffer, 0); // "unfreeze" GPU, allowing it read access again

	//bind to vertex Shader
	Graphics::Context->VSSetConstantBuffers(0, 1, &vertexConstBuffer);
}
