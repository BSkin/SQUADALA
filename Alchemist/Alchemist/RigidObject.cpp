#include "RigidObject.h"

btAlignedObjectArray<btCollisionShape*> *	RigidObject::collisionShapes =	NULL;
btDiscreteDynamicsWorld *					RigidObject::dynamicsWorld =	NULL;

int RigidObject::initBullet()
{
	colShape = new btBoxShape(btVector3(width,height,1));
	btStridingMeshInterfaceData asdf;
	//
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
	body->setFriction(0.5f);

	return 0;
}

void RigidObject::setBullet(btAlignedObjectArray<btCollisionShape*> * col, btDiscreteDynamicsWorld * world)
{
	collisionShapes = col;
	dynamicsWorld = world;
}