#include "RigidObject.h"

btAlignedObjectArray<btCollisionShape*> *	RigidObject::collisionShapes =	NULL;
btDiscreteDynamicsWorld *					RigidObject::dynamicsWorld =	NULL;

RigidObject::RigidObject() : GameObject(), mass(1.0f), speed(0), physInit(false)
{}

RigidObject::RigidObject(char *fileBase) : GameObject(fileBase), mass(1.0f), speed(0), physInit(false)
{}

RigidObject::RigidObject(short ID) : GameObject(ID), mass(1.0f), speed(0), physInit(false)
{}

RigidObject::~RigidObject()
{}

int RigidObject::initGeom()
{
	return GameObject::initGeom();
}

int RigidObject::initBullet()
{
	colShape = new btBoxShape(btVector3(width*0.5,height*0.5, 0.5));
	//colShape = new btSphereShape(btScalar(0.1));

	collisionShapes->push_back(colShape);

	//colShape->setMargin(btScalar(0.03));

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(
					btScalar(position.x),
					btScalar(position.y),
					btScalar(0.1)));

	startTransform.setRotation(btQuaternion(btVector3(0,1,0), 3.14159265358979323846264338327950288419716939));
	startTransform.setRotation(btQuaternion(3.14159265358979323846264338327950288419716939, 0, rotation));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
		

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	body = new btRigidBody(rbInfo);

	//body->setActivationState(ISLAND_SLEEPING);
	body->setLinearFactor(btVector3(1,1,0));
	body->setAngularFactor(btVector3(0,0,1));
	body->setFriction(0.5f);

	dynamicsWorld->addRigidBody(body);
	
	body->activate(true);
	body->setLinearVelocity(btVector3(cos(rotation) * speed, -sin(rotation) * speed, 0));

	//body->setActivationState(ISLAND_SLEEPING);

	return 0;
}

void RigidObject::setMass(float x) { mass = x; }
void RigidObject::setSpeed(float x) { speed = x; }

int RigidObject::update(long time)
{
	if (!physInit) 
	{ initBullet(); physInit = true;	}
	//if (body->getCenterOfMassPosition().y() < -2) body->translate(btVector3(0,7,0));
	return GameObject::update(time);
}

int RigidObject::renderFrame(long time)
{
	if (!physInit) 
	{ initBullet(); physInit = true; }
	D3DXMATRIX trans = Utility::btTransToD3DXMat(&body->getWorldTransform());
	GameObject::renderFrame(time, &trans);

	position = Utility::ConvertToD3D(&body->getCenterOfMassPosition());
	renderPosition = position - D3DXVECTOR3(camera->getPosition().x, camera->getPosition().y, 0);

	return 0;
}

void RigidObject::setBullet(btAlignedObjectArray<btCollisionShape*> * col, btDiscreteDynamicsWorld * world)
{
	collisionShapes = col;
	dynamicsWorld = world;
}