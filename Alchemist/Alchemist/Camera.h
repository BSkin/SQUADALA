#pragma once
#include <d3d9.h>
#include <d3dx9.h>
       
//#include <windows.h>

//#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>

#define FIRST_PERSON 0
#define THIRD_PERSON 1
#define THIRD_PERSON_FIXED 2

class Camera
{
public:
	Camera(void);
	~Camera(void);
 
	void getProjMatrix(D3DXMATRIX * projMatrix);
	void getViewMatrix(D3DXMATRIX * viewMatrix);
	void setProj(float nearPlane, float farPlane, float fieldOfView, float aspectRatio);

	void toggleOrth(void);
	void setType(short);
	void setOrth(bool);
	void setDimensions(int, int);

	void setWorldPointer(D3DXMATRIX *);

	void turn(long x, long y);
	void setPosition(D3DXVECTOR3);
	void setPosition(float, float, float);
	D3DXVECTOR3 getPosition(void);
	D3DXVECTOR3 getLookAtVector(void);
	D3DXVECTOR3 getUpVector(void);
	void modPosition(float, float, float);
	void modPosition(D3DXVECTOR3);
	D3DXVECTOR2 getRotation();

	void moveForward(void);
	void moveLeft(void);
	void moveRight(void);
	void moveBackward(void);

private:
	void updateCamera();

	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 position;
	D3DXVECTOR3 lookAtVector;
	
	const float PI;

	short cameraType;
	float horizontalRotation;
	float verticalRotation;
	float cameraDistance;

	float speed;

	D3DXMATRIX * worldMatrix;

	bool orthogonalView;
	float nearPlane;
	float farPlane;
	float fieldOfView;
	float aspectRatio;
	int wndWidth, wndHeight;
};