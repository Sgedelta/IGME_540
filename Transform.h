#pragma once

#include <DirectXMath.h>

class Transform
{

public:


	Transform();
	~Transform();
	Transform(const Transform&) = delete; // Remove copy constructor
	Transform& operator=(const Transform&) = delete; // Remove copy-assignment operator

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTranspose();



	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void MoveRelative(DirectX::XMFLOAT3 offset);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

private:

	void RecalculateWorldAndTranspose();
	void RecalculateLocalDirections();

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation; //doing rotation as a vector for now, may change to matrix eventually.
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT3 localRight;
	DirectX::XMFLOAT3 localUp;
	DirectX::XMFLOAT3 localForward;


	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTranspose;

	bool dirty = true;
	bool rotDirty = false;
};

