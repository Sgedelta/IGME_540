#pragma once
#include <memory>
#include <wrl/client.h>
#include <ctime>
#include <chrono>

#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"


class Entity
{

public:
	Entity(std::shared_ptr<Mesh> meshPtr, std::shared_ptr<Material> mat);
	~Entity();
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> matPtr);

	void Draw( float tint[4], Camera* cameraPtr);
	void DrawForLight();

private:

	std::shared_ptr<Transform> sharedTransform;
	std::shared_ptr<Mesh> sharedMesh;
	std::shared_ptr<Material> sharedMaterial;



	void SendGPUData( float tint[4], Camera* cameraPtr);
	std::chrono::system_clock::time_point start;
	
};

