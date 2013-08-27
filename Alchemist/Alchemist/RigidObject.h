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

	static void setBullet(btAlignedObjectArray<btCollisionShape*> *, btDiscreteDynamicsWorld *);

private:
	virtual int initGeom();
	virtual int initBullet();

	static btAlignedObjectArray<btCollisionShape*> * collisionShapes;
	static btDiscreteDynamicsWorld * dynamicsWorld;

	btBoxShape* colShape;
	btRigidBody * body;
	bool physInit;
};