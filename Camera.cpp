#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float aspectRatio, XMFLOAT3 startingPosition)
{
	transformPtr = std::make_shared<Transform>();
	transformPtr->SetPosition(startingPosition);


	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMat;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMat;
}

void Camera::Update(float dt)
{
	if (Input::KeyDown('W')) {
		transformPtr.get()->MoveRelative(0, 0, dt * moveSpeed * ( Input::KeyDown(VK_SHIFT) ? 5 : 1 ));
	}
	if (Input::KeyDown('S')) {
		transformPtr.get()->MoveRelative(0, 0, dt * -moveSpeed * (Input::KeyDown(VK_SHIFT) ? 5 : 1));
	}
	
	if (Input::KeyDown('D')) {
		transformPtr.get()->MoveRelative(dt * moveSpeed * (Input::KeyDown(VK_SHIFT) ? 5 : 1), 0, 0);
	}
	if (Input::KeyDown('A')) {
		transformPtr.get()->MoveRelative(dt * -moveSpeed * (Input::KeyDown(VK_SHIFT) ? 5 : 1), 0, 0);
	}

	if (Input::KeyDown(VK_SPACE)) {
		transformPtr.get()->MoveAbsolute(0, dt * moveSpeed * (Input::KeyDown(VK_SHIFT) ? 5 : 1), 0);
	}
	if (Input::KeyDown('X')) {
		transformPtr.get()->MoveAbsolute(0, dt * -moveSpeed * (Input::KeyDown(VK_SHIFT) ? 5 : 1), 0);
	}

	if (Input::MouseLeftDown()) {
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();

		transformPtr.get()->Rotate(cursorMovementY * mouseLookSpeed, cursorMovementX * mouseLookSpeed, 0);
		if (transformPtr.get()->GetPitchYawRoll().y > XM_PIDIV2 ) {
			transformPtr.get()->SetRotation(0, XM_PIDIV2, 0);
		}
		else if(transformPtr.get()->GetPitchYawRoll().y < -XM_PIDIV2) {
			transformPtr.get()->SetRotation(0, -XM_PIDIV2, 0);
		}
	}

	UpdateViewMatrix();

	//printf("%f, %f, %f", transformPtr.get()->GetPosition().x, transformPtr.get()->GetPosition().y, transformPtr.get()->GetPosition().z);
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transformPtr.get()->GetPosition();
	XMFLOAT3 forward = transformPtr.get()->GetForward();
	XMStoreFloat4x4(&viewMat, XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&forward), XMVECTOR({ 0, 1, 0 })));
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	if (isPerspective) {
		XMStoreFloat4x4(&projectionMat, XMMatrixPerspectiveFovLH(FoV, aspectRatio, nearClip, farClip));
	}
}
