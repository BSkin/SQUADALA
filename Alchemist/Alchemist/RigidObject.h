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

	void setMass(float x);
	void setSpeed(float x);
	static void setBullet(btAlignedObjectArray<btCollisionShape*> *, btDiscreteDynamicsWorld *);
	btRigidBody * body;
protected:
	virtual int initGeom();
	virtual int initBullet();

	static btAlignedObjectArray<btCollisionShape*> * collisionShapes;
	static btDiscreteDynamicsWorld * dynamicsWorld;

	btCollisionShape* colShape;
	//btRigidBody * body;
	bool physInit;

	btScalar mass;
	btScalar speed;
};