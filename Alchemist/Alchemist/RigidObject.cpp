#include "RigidObject.h"

btAlignedObjectArray<btCollisionShape*> *	RigidObject::collisionShapes =	NULL;
btDiscreteDynamicsWorld *					RigidObject::dynamicsWorld =	NULL;

RigidObject::RigidObject() : GameObject(), mass(1.0f), physInit(false)
{}

RigidObject::RigidObject(char *fileBase) : GameObject(fileBase), mass(1.0f), physInit(false)
{}

RigidObject::RigidObject(short ID) : GameObject(ID), mass(1.0f), physInit(false)
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

	dynamicsWorld->addRigidBody(body);

	//body->setActivationState(ISLAND_SLEEPING);

	return 0;
}

void RigidObject::setMass(float x) { mass = x; }

int RigidObject::update(long time)
{
	if (!physInit) 
	{ initBullet(); physInit = true;	}
	return GameObject::update(time);
}

int RigidObject::renderFrame(long time)
{
	if (!physInit) 
	{ initBullet(); physInit = true; }
	D3DXMATRIX trans = Utility::btTransToD3DXMat(&body->getWorldTransform());
	//return GameObject::renderFrame(time);
	return GameObject::renderFrame(time, &trans);

}

void RigidObject::setBullet(btAlignedObjectArray<btCollisionShape*> * col, btDiscreteDynamicsWorld * world)
{
	collisionShapes = col;
	dynamicsWorld = world;
}