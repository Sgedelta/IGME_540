#include "Transform.h"

#include <DirectXMath.h>

using namespace DirectX;

Transform::Transform()
{
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());


}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = XMFLOAT3(position.x, position.y, position.z);
	dirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3(pitch, yaw, roll);
	dirty = true;
	rotDirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation = XMFLOAT3(position.x, position.y, position.z);
	dirty = true;
	rotDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	dirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = XMFLOAT3(position.x, position.y, position.z);
	dirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (!dirty) {
		return worldMatrix;
	}

	//calculate world and inverseTranspose
	RecalculateWorldAndTranspose();

	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTranspose()
{
	if (!dirty) {
		return worldInverseTranspose;
	}

	//calculate world and inverseTranspose
	RecalculateWorldAndTranspose();

	return worldInverseTranspose;
}

void Transform::RecalculateWorldAndTranspose()
{
	XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	XMMATRIX world = scaleMat * rotationMat * translationMat; //uses overloaded operators

	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(world)));

        dirty = false;
}


void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	dirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;
	dirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR newVec = XMVECTOR({ x, y, z });
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	newVec = XMVector3Rotate(newVec, rotationQuat);

	position.x += XMVectorGetX(newVec);
	position.y += XMVectorGetY(newVec);
	position.z += XMVectorGetZ(newVec);
	dirty = true;
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	MoveRelative(offset.x, offset.y, offset.z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
	dirty = true;
	rotDirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	this->rotation.x += rotation.x;
	this->rotation.y += rotation.y;
	this->rotation.z += rotation.z;
	dirty = true;
	rotDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	dirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
	dirty = true;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	if (rotDirty) {
		RecalculateLocalDirections();
	}

	return localRight;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	if (rotDirty) {
		RecalculateLocalDirections();
	}

	return localUp;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	if (rotDirty) {
		RecalculateLocalDirections();
	}

	return localForward;
}

void Transform::RecalculateLocalDirections()
{
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR rotatedRight = XMVector3Rotate(XMVECTOR({ 1, 0, 0 }), rotationQuat);
	XMVECTOR rotatedUp = XMVector3Rotate(XMVECTOR({0, 1, 0}), rotationQuat);
	XMVECTOR rotatedForward = XMVector3Rotate(XMVECTOR({0, 0, 1}), rotationQuat);

	localRight = XMFLOAT3(XMVectorGetX(rotatedRight), XMVectorGetY(rotatedRight), XMVectorGetZ(rotatedRight));
	localUp = XMFLOAT3(XMVectorGetX(rotatedUp), XMVectorGetY(rotatedUp), XMVectorGetZ(rotatedUp));
	localForward = XMFLOAT3(XMVectorGetX(rotatedForward), XMVectorGetY(rotatedForward), XMVectorGetZ(rotatedForward));

	rotDirty = false;
}
