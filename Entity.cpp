#include "Entity.h"

#include <memory>

#include "Transform.h"
#include "Mesh.h"

using namespace DirectX;

Entity::Entity(Mesh* mesh)
{
	sharedMesh = std::make_shared<Mesh>(); //don't think this is right.
	sharedTransform = std::make_shared<Transform>();
}

Entity::~Entity()
{
}
