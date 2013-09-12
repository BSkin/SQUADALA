#pragma once

#include "GameObject.h"

class RigidObject : public GameObject
{
public:
	RigidObject(void);
	RigidObject(char *fileBase);
	RigidObject(short ID);
	~RigidObject(void);

	virtual int update(long time);
	virtual int renderFrame(long time);
	virtual int collide(RigidObject *, const btVector3 * worldCollPos);
	virtual int applyForce(const btVector3 * velocity, float mass, const btVector3 * worldPos);

	const btCollisionObject * getBody();
	void setMass(float x);
	void setSpeed(float x);
	void setGravity(float x);
	static void setStatics(btAlignedObjectArray<btCollisionShape*> *, btDiscreteDynamicsWorld *, list<RigidObject *> *);
protected:
	virtual int initGeom();
	virtual int initBullet();

	static btAlignedObjectArray<btCollisionShape*> * collisionShapes;
	static btDiscreteDynamicsWorld * dynamicsWorld;
	static list<RigidObject *> * physObjects;

	btCollisionShape* colShape;
	btRigidBody * body;
	bool physInit;

	btScalar mass;
	btScalar speed;
	btScalar gravity;
};