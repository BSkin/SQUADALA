#include "Player.h"

RawInputManager *	Player::inputManager =	NULL;

Player::Player(void) : Character()
{
	
}

Player::Player(char *fileBase) : Character(fileBase)
{
	position = D3DXVECTOR3(0,0,0);
}

Player::Player(short ID) : Character(ID)
{
	
}

Player::~Player(void)
{
	
}

/*
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
*/

int Player::initGeom()
{
	GameObject::initGeom();
	//body->activate(true);
	//body->translate(btVector3(position.x, position.y, position.z));
	return 0;
}

int Player::update(long time)
{
	//if (inputManager->getKey('W') > 0) moveUp();
	//if (inputManager->getKey('S') > 0) moveDown();
	acceleration = D3DXVECTOR3(0,-0.167, 0);

	velocity += acceleration;
	direction = D3DXVECTOR3(0,0,0);	

	if (inputManager->getKey('W') > 0) moveUp();
	if (inputManager->getKey('S') > 0) moveDown();
	if (inputManager->getKey('D') > 0) moveRight();
	if (inputManager->getKey('A') > 0) moveLeft();

	position.x += speed * direction.x;

	if (position.y <= 0)
	{
		velocity = D3DXVECTOR3(0,0,0);
		position.y = 0;

		direction.y = 1;
		float jumpSpeed = speed;
		if (direction.x) jumpSpeed *= 0.70710678f;
		direction.y++;
		D3DXVECTOR3 jumpDirection = jumpSpeed*direction;
		//jumpDirection.y += finalSpeed;

		if (inputManager->getKey(VK_SPACE) > 0) velocity += jumpDirection;
	}
	position += velocity;


	//body->activate(true);
	//body->translate(btVector3(velocity.x, velocity.y, velocity.z));
	
	/*
	#pragma region Calculate World Matrix
	D3DXMATRIX scaleMatrix, translationMatrix, rotationMatrix;
	
	D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
	D3DXMatrixRotationY(&rotationMatrix, horizontalRotation+3.14159f);
	
	worldMatrix = rotationMatrix * translationMatrix;
	#pragma endregion
	
	#pragma region Set Physics Object Transform
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
	*/

	return 0;
}

int Player::renderFrame(long time)
{
	/*if (!physInit)
	{
		initBullet();
		physInit = true;
	}*/
	quad.setPos(position);
	GameObject::renderFrame(time);
	return 0;
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