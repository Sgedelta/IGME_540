#include "Entity.h"

#include <memory>
#include <wrl/client.h>
#include <ctime>
#include <chrono>

#include "Transform.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Graphics.h"
#include "SimpleShader.h"
#include "Material.h"

using namespace DirectX;

Entity::Entity(std::shared_ptr<Mesh> meshPtr, std::shared_ptr<Material> mat)
{
	sharedMesh = meshPtr;
	sharedMaterial = mat;
	sharedTransform = std::make_shared<Transform>();

	start = std::chrono::system_clock::now();
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

std::shared_ptr<Material> Entity::GetMaterial()
{
	return sharedMaterial;
}

void Entity::SetMaterial(std::shared_ptr<Material> matPtr)
{
	sharedMaterial = matPtr;
}

void Entity::Draw( float tint[4], Camera* cameraPtr)
{
	SendGPUData(tint, cameraPtr);
	sharedMesh.get()->Draw();
}

void Entity::SendGPUData( float tint[4], Camera* cameraPtr)
{
	//bind our shaders:
	sharedMaterial->GetVertexShader()->SetShader();
	sharedMaterial->GetPixelShader()->SetShader();

	
	float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() / 1000.0f;

	XMFLOAT4 combinedTint = XMFLOAT4(
		sharedMaterial->GetColorTint().x * tint[0],
		sharedMaterial->GetColorTint().y * tint[1],
		sharedMaterial->GetColorTint().z * tint[2],
		sharedMaterial->GetColorTint().w * tint[4]
	);
	switch (sharedMaterial->GetMaterialType()) {

	case 2:
		sharedMaterial->GetPixelShader()->SetFloat("timeInSeconds", elapsedTime);
		break;

	case 1:
		
		sharedMaterial->GetPixelShader()->SetFloat4("colorTint", combinedTint);
		break;

	case 0:
	default:
		break;
	}
	

	sharedMaterial->GetVertexShader()->SetFloat2("uv", XMFLOAT2(0, 0));
	sharedMaterial->GetVertexShader()->SetFloat3("normal", XMFLOAT3(0, 0, 0));
	sharedMaterial->GetVertexShader()->SetMatrix4x4("worldMatrix", sharedTransform.get()->GetWorldMatrix());
	sharedMaterial->GetVertexShader()->SetMatrix4x4("viewMatrix", cameraPtr->GetViewMatrix() );
	sharedMaterial->GetVertexShader()->SetMatrix4x4("projectionMatrix", cameraPtr->GetProjectionMatrix());

	//actually send data we bound
	sharedMaterial->GetPixelShader()->CopyAllBufferData();
	sharedMaterial->GetVertexShader()->CopyAllBufferData(); 

}
