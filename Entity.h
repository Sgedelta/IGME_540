#pragma once
#include <memory>

#include "Transform.h"
#include "Mesh.h"

class Entity
{

public:
	Entity(Mesh* mesh);
	~Entity();
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;



private:

	std::shared_ptr<Transform> sharedTransform;
	std::shared_ptr<Mesh> sharedMesh;
};

