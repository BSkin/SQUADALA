#include "camera.h"
#include <cmath>

Camera::Camera(void) : position(0.0,0.0,-1000.0), lookAt(0.0,0.0,0.0), 
	orthogonalView(true), 
	cameraType(CAMERA_2D), 
	speed(1.0f), 
	worldMatrix(0),
	PI(atan(1)*4), verticalRotation(0.0f), horizontalRotation(0.0f), 
	nearPlane(1.0f), farPlane(1000.0f),
	cameraDistance(25.0f),
	wndWidth(1280), wndHeight(720)
{

}

Camera::~Camera(void)
{

}

void Camera::getViewMatrix(D3DXMATRIX * viewMatrix)
{
	if (orthogonalView || cameraType == CAMERA_2D)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3(0,0,-500);
		D3DXVECTOR3 zero = D3DXVECTOR3(0,0,0);
		D3DXVECTOR3 u = D3DXVECTOR3(0,1,0);
		D3DXMatrixLookAtLH(viewMatrix, &pos, &zero, &u);
	}
	else
	{

		D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);

		updateCamera();

		D3DXMatrixLookAtLH(viewMatrix, &position, &lookAt, &up);
		
		if (worldMatrix && cameraType == FIRST_PERSON) 
		{
			//*viewMatrix = *worldMatrix;
			
			D3DXMatrixInverse(viewMatrix, NULL, worldMatrix);
		}

		/*
		D3DXMATRIX p, r;
		D3DXMatrixTranslation(&p, -position.x, -position.y, -position.z);
		D3DXMatrixRotationY(&r, 0.0f);

		*viewMatrix = r * p;
		*/
	}
}  

void Camera::getProjMatrix(D3DXMATRIX * matProj)
{
	if (orthogonalView == true || cameraType == CAMERA_2D)
		D3DXMatrixOrthoLH(matProj, wndWidth, wndHeight, nearPlane, farPlane);
	else 
		D3DXMatrixPerspectiveFovLH(matProj, fieldOfView, aspectRatio, nearPlane, farPlane);
}

void Camera::setProj(float nearPlane, float farPlane, float fieldOfView, float aspectRatio)
{
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->fieldOfView = fieldOfView;
	this->aspectRatio = aspectRatio;
}

void Camera::setType(short x) { cameraType = x; }
void Camera::setDimensions(int x, int y) { wndWidth = x; wndHeight = y; }
void Camera::setOrth(bool x) { orthogonalView = x; }
void Camera::toggleOrth() { orthogonalView = !orthogonalView; }

void Camera::setWorldPointer(D3DXMATRIX * x) { worldMatrix = x; }

void Camera::turn(long x, long y)
{
	horizontalRotation += x*0.0025;
	verticalRotation += y*0.0025;

	if (verticalRotation > (PI/2 - 0.01f)) verticalRotation = PI/2 - 0.01f;
	if (verticalRotation < -(PI/2 - 0.01f)) verticalRotation = -PI/2 + 0.01f;

	//if (horizontalRotation > 2*PI) horizontalRotation -= 2*PI;
	//if (horizontalRotation < -2*PI) horizontalRotation += 2*PI;
}

D3DXVECTOR3 Camera::getPosition() 
{ 
	if (cameraType == CAMERA_2D || cameraType == FIRST_PERSON) 
		return position; 
	else if (cameraType == THIRD_PERSON)
		return lookAt;
}
D3DXVECTOR3 Camera::getLookAtVector(void) { return  lookAtVector; }
D3DXVECTOR3 Camera::getUpVector()
{
	D3DXVECTOR3 up;
	D3DXVECTOR3 temp = D3DXVECTOR3(0,1,0);
	D3DXVec3Cross(&up, &lookAtVector, &temp);
	D3DXVec3Cross(&up, &lookAtVector, &up);
	D3DXVec3Normalize(&up, &up);
	return -up;
}
void Camera::modPosition(D3DXVECTOR3 vector) 
{ 
	if (cameraType == THIRD_PERSON)
		lookAt += vector;
	else position += vector;
	updateCamera();
}
void Camera::modPosition(float x, float y, float z) { modPosition(D3DXVECTOR3(x,y,z)); }
void Camera::setPosition(D3DXVECTOR3 target) 
{ 
	if (cameraType == THIRD_PERSON)
		lookAt = target; 
	else position = target*0.01;
	//else if (cameraType == CAMERA_2D)
	//	position = D3DXVECTOR3(0,0,-10);
}
void Camera::setPosition(float x, float y, float z) { setPosition(D3DXVECTOR3(x,y,z)); }

D3DXVECTOR2 Camera::getRotation() { return D3DXVECTOR2(horizontalRotation, verticalRotation); }

#pragma region Move Functions
void Camera::moveForward()
{
	D3DXVECTOR3 temp = lookAt-position;
	temp.y = 0;
	D3DXVec3Normalize(&temp, &temp);
	if (cameraType == FIRST_PERSON)
		position += temp*1;
	else if (cameraType == THIRD_PERSON)
		lookAt += temp*1.0f;
	updateCamera();
}
void Camera::moveBackward()
{
	D3DXVECTOR3 temp = lookAt-position;
	temp.y = 0;
	D3DXVec3Normalize(&temp, &temp);
	if (cameraType == FIRST_PERSON)
		position -= temp*1;
	else if (cameraType == THIRD_PERSON)
		lookAt -= temp*1.0f;
	updateCamera();
}
void Camera::moveLeft()
{
	D3DXVECTOR3 temp = lookAt-position;
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f,1.0f,0.0f);
	temp.y = 0;
	D3DXVec3Normalize(&temp, &temp);
	D3DXVec3Cross(&temp, &temp, &up);
	if (cameraType == FIRST_PERSON)
		position += temp*1;
	else if (cameraType == THIRD_PERSON)
		lookAt += temp*1.0f;
	updateCamera();
}
void Camera::moveRight()
{
	D3DXVECTOR3 temp = lookAt-position;
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f,1.0f,0.0f);
	temp.y = 0;
	D3DXVec3Normalize(&temp, &temp);
	D3DXVec3Cross(&temp, &temp, &up);
	if (cameraType == FIRST_PERSON)
		position -= temp*1;
	else if (cameraType == THIRD_PERSON)
		lookAt -= temp*1.0f;
	updateCamera();
}
#pragma endregion

void Camera::updateCamera()
{
	if (cameraType == FIRST_PERSON)
	{
		lookAt = position;
		lookAt.y -= sin(verticalRotation);
		lookAt.x -= sin(horizontalRotation) * cos(verticalRotation);
		lookAt.z -= cos(horizontalRotation) * cos(verticalRotation);
	}
	else if (cameraType == THIRD_PERSON)
	{
		position = lookAt;
		position.y += sin(verticalRotation) * cameraDistance;
		position.x += sin(horizontalRotation) * cos(verticalRotation) * cameraDistance;
		position.z += cos(horizontalRotation) * cos(verticalRotation) * cameraDistance;
	}
	else if (cameraType == THIRD_PERSON_FIXED)
	{
		verticalRotation = PI*0.35f;
		horizontalRotation = PI*0.3f;
		position = lookAt;
		position.y += sin(verticalRotation) * cameraDistance;
		position.x += sin(horizontalRotation) * cos(verticalRotation) * cameraDistance;
		position.z += cos(horizontalRotation) * cos(verticalRotation) * cameraDistance;
	}
	else if (cameraType == CAMERA_2D)
	{
		verticalRotation = 0;
		horizontalRotation = 0;
		//position = D3DXVECTOR3(0,0,-10);
		lookAt = position + D3DXVECTOR3(0,0,100);
	}
	
	lookAtVector = lookAt - position;
	D3DXVec3Normalize(&lookAtVector, &lookAtVector);
}