#include "Player.h"

RawInputManager *	Player::inputManager =	NULL;
Camera *			Player::camera =		NULL;

Player::Player(void) : GameObject()
{
	
}

Player::Player(char *fileBase) : GameObject(fileBase)
{
	position = D3DXVECTOR3(0,0,0);
}

Player::Player(short ID) : GameObject(ID)
{
	
}

Player::~Player(void)
{
	
}

int Player::initBullet()
{
	float SCALE = 1;
	
	colShape = new btBoxShape(btVector3(scale.x,scale.y,scale.z));
	//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	collisionShapes->push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.f);
		
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
		
	startTransform.setOrigin(btVector3(
					btScalar(position.x),
					btScalar(position.y),
					btScalar(position.z)));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);

	//body->setGravity(btVector3(0,0,0));
	body->setFriction(0.5f);

	return 0;
}

int Player::initGeom()
{
	camera->setWorldPointer(&worldMatrix);
	GameObject::initGeom();
	//body->activate(true);
	//body->translate(btVector3(position.x, position.y, position.z));
	return 0;
}

int Player::update(long time)
{
	if (!physInit)
	{
		initBullet();
		physInit = true;
	}
	//body->activate(true);
	
	
	setRotation(camera->getRotation());
	setAcceleration(0, -0.05f, 0);
	
	float y = scale.y;//10 + ocean->getHeight(position, time);

	if (position.y <= y)
	{
		position.y = y;
		setVelocity(0,0,0);

		if (inputManager->getKey('W')) moveForward();
		if (inputManager->getKey('S')) moveBackward();
		if (inputManager->getKey('D')) moveRight();
		if (inputManager->getKey('A')) moveLeft();
			
		if (inputManager->getKey(VK_SPACE)) modVelocity(0,1,0);
	}

	velocity += acceleration;
	position += velocity;
	
	/*if (position.y <= y && velocity.y < 0)
	{
		position.y = y;
	    velocity.y = 0;
	}*/

	//position.y = sin(time*0.01+offset)*10;
	

	//GameObject::update(time);

	//body->activate(true);
	//body->translate(btVector3(velocity.x, velocity.y, velocity.z));
	
	#pragma region Calculate World Matrix
	D3DXMATRIX scaleMatrix, translationMatrix, rotationMatrix;
	
	D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
	D3DXMatrixRotationY(&rotationMatrix, horizontalRotation+3.14159f);
	
	worldMatrix = rotationMatrix * translationMatrix;
	#pragma endregion
	
	#pragma region Set Physics Object Transform
	/*
	btTransform * bulletTransformMatrix = &body->getWorldTransform();
	btVector3 R = bulletTransformMatrix->getBasis().getColumn(0);
	btVector3 U = bulletTransformMatrix->getBasis().getColumn(1);
	btVector3 L = bulletTransformMatrix->getBasis().getColumn(2);
	btVector3 P = bulletTransformMatrix->getOrigin();
				
	D3DXVECTOR3 vR, vU, vL, vP;
	vR.x = R.x();vR.y = R.y();vR.z = R.z();
	vU.x = U.x();vU.y = U.y();vU.z = U.z();
	vL.x = L.x();vL.y = L.y();vL.z = L.z();
	vP.x = P.x();vP.y = P.y();vP.z = P.z();

	D3DXMATRIX matOutput;
	matOutput._11 = vR.x;matOutput._12 = vR.y;matOutput._13 = vR.z;matOutput._14 = 0.f;
	matOutput._21 = vU.x;matOutput._22 = vU.y;matOutput._23 = vU.z;matOutput._24 = 0.f;
	matOutput._31 = vL.x;matOutput._32 = vL.y;matOutput._33 = vL.z;matOutput._34 = 0.f;
	matOutput._41 = vP.x;matOutput._42 = vP.y;matOutput._43 = vP.z;matOutput._44 = 1.f;
	*/

	
	btTransform bulletTransformMatrix;
	btVector3 R,U,L,P;
	R.setX(btScalar(worldMatrix._11)); R.setY(btScalar(worldMatrix._12)); R.setZ(btScalar(worldMatrix._13));
	U.setX(btScalar(worldMatrix._21)); U.setY(btScalar(worldMatrix._22)); U.setZ(btScalar(worldMatrix._23));
	L.setX(btScalar(worldMatrix._31)); L.setY(btScalar(worldMatrix._32)); L.setZ(btScalar(worldMatrix._33));
	P.setX(btScalar(worldMatrix._41)); P.setY(btScalar(worldMatrix._42)); P.setZ(btScalar(worldMatrix._43));

	btMatrix3x3 bMatrix;
	bMatrix = btMatrix3x3(R.x(), R.y(), R.z(), U.z(), U.y(), U.z(), L.x(), L.y(), L.z());
	bulletTransformMatrix.setBasis(bMatrix);
	bulletTransformMatrix.setOrigin(P);

	body->setWorldTransform(bulletTransformMatrix);
	body->activate(true);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	#pragma endregion
	

	//GameObject::update(time);

	return 0;
}

int Player::renderFrame(long time)
{
	if (!physInit)
	{
		initBullet();
		physInit = true;
	}
	return GameObject::renderFrame(time);
}

int Player::setInputManager(RawInputManager * iManager)
{
	if (iManager) 
	{
		inputManager = iManager;
		return 0;
	}
	return -1;
}

int Player::setCamera(Camera * cam)
{
	if (cam)
	{
		camera = cam;
		return 0;
	}
	return -1;
}