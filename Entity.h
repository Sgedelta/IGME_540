#pragma once
#include <memory>
#include <wrl/client.h>

#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"

class Entity
{

public:
	Entity(std::shared_ptr<Mesh> meshPtr);
	~Entity();
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();

	void Draw(ID3D11Buffer* vertexConstBuffer, float tint[4], Camera* cameraPtr);

private:

	std::shared_ptr<Transform> sharedTransform;
	std::shared_ptr<Mesh> sharedMesh;



	void SendGPUData(ID3D11Buffer* vertexConstBuffer, float tint[4], Camera* cameraPtr);
	
};

