#pragma once

#include "btBulletDynamicsCommon.h"
#include "RigidBody.h"

class DynamicsWorld : public btDiscreteDynamicsWorld
{
public:
	DynamicsWorld(btCollisionDispatcher*, btBroadphaseInterface*, btConstraintSolver*, btDefaultCollisionConfiguration*);

	virtual void clearForces();
};