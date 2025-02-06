#include "Transform.h"

#include <DirectXMath.h>

using namespace DirectX;

XMFLOAT4X4 Transform::GetWorldMatrix()
{
    if (dirty) {
        XMMATRIX triMatrix;
        dirty = false;
    }

    return worldMatrix;
}
