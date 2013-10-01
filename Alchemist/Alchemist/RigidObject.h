#pragma once

#include "GameObject.h"
#include "btRigidBodyEx.h"

class RigidObject : public GameObject
{
public:
	RigidObject(void);
	RigidObject(char *fileBase);
	RigidObject(short ID);
	virtual ~RigidObject(void);

	virtual int update(long time);
	virtual int renderFrame(long time);
	virtual int collide(GameObject *, const btVector3 * worldCollPos);
	virtual int applyForce(const btVector3 * velocity, float mass, const btVector3 * worldPos);

	virtual const btCollisionObject * getBody();
	//virtual D3DXVECTOR3 getPosition();

	void setMass(float x);
	void setSpeed(float x);
	void setGravity(float x);
	static void setStatics(btAlignedObjectArray<btCollisionShape*> *, btDiscreteDynamicsWorld *);
protected:
	virtual int initGeom();
	virtual int initBullet();

	static btAlignedObjectArray<btCollisionShape*> * collisionShapes;
	static btDiscreteDynamicsWorld * dynamicsWorld;

	btDefaultMotionState* myMotionState;
	btCollisionShape* colShape;
	btRigidBodyEx * body;
	bool physInit;

	btScalar mass;
	btScalar speed;
	btScalar gravity;
};