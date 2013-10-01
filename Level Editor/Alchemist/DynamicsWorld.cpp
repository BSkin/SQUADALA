#include "DynamicsWorld.h"

DynamicsWorld::DynamicsWorld(btCollisionDispatcher *a, btBroadphaseInterface *b, btConstraintSolver *c, btDefaultCollisionConfiguration *d) : btDiscreteDynamicsWorld(a,b,c,d)
{}

void DynamicsWorld::clearForces()
{
	for ( int i=0;i<m_nonStaticRigidBodies.size();i++)
	{
		btRigidBody* body = m_nonStaticRigidBodies[i];
		RigidBody * b = static_cast<RigidBody*>(body);
		//need to check if next line is ok
		//it might break backward compatibility (people applying forces on sleeping objects get never cleared and accumulate on wake-up
		b->saveAndClearForces();
	}
}