#pragma once

#include <DirectXMath.h>

class Transform
{

public:
	Transform();
	Transform(const Transform& other);
	~Transform();


	DirectX::XMFLOAT4X4 GetWorldMatrix();

	void Rotate(float p, float y, float r); // using pitch yaw roll for now - could do quaternions if I wanted
	void Scale(float x, float y, float z);

	void SetPosition(float x, float y, float z);
	void Translate(float x, float y, float z);
	void TranslateLocal(float x, float y, float z);


private:

	bool dirty; //if any of the values have changed, we may need to recalculate world matrix
	DirectX::XMFLOAT4X4 worldMatrix;

};

