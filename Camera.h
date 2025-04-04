#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

class Camera
{
public:

	Camera(float aspectRatio, DirectX::XMFLOAT3 startingPosition);
	~Camera();
	Camera(const Camera&) = delete; // Remove copy constructor
	Camera& operator=(const	Camera&) = delete; // Remove copy-assignment operator

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	std::shared_ptr<Transform> GetTransform();
	void UpdateProjectionMatrix(float aspectRatio);

	void Update(float dt);

private:

	std::shared_ptr<Transform> transformPtr;
	DirectX::XMFLOAT4X4 viewMat;
	DirectX::XMFLOAT4X4 projectionMat;

	float FoV = 1.5707f; //in radians
	float nearClip = .01f;
	float farClip = 500;
	float moveSpeed = 1;
	float mouseLookSpeed = .01f; 
	bool isPerspective = true;

	void UpdateViewMatrix();



};

