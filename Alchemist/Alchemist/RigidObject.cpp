#include "RigidObject.h"

btAlignedObjectArray<btCollisionShape*> *	RigidObject::collisionShapes =	NULL;
btDiscreteDynamicsWorld *					RigidObject::dynamicsWorld =	NULL;
list<RigidObject *> *						RigidObject::physObjects =		NULL;

RigidObject::RigidObject() : GameObject(), mass(1.0f), speed(0), gravity(-9.8), physInit(false)
{}

RigidObject::RigidObject(char *fileBase) : GameObject(fileBase), mass(1.0f), speed(0), gravity(-9.8), physInit(false)
{}

RigidObject::RigidObject(short ID) : GameObject(ID), mass(1.0f), speed(0), gravity(-9.8), physInit(false)
{}

RigidObject::~RigidObject()
{
	if (body) dynamicsWorld->removeRigidBody(body);
	if (colShape) collisionShapes->remove(colShape);
	physObjects->remove(this);

	if (body && body->getMotionState())
	{
		delete body->getMotionState();
	}
	delete body;
	body = 0;
}

int RigidObject::initGeom()
{
	return GameObject::initGeom();
}

int RigidObject::initBullet()
{
	colShape = new btBoxShape(btVector3(width*0.5,height*0.5, 10));
	//colShape = new btSphereShape(btScalar(0.1));

	collisionShapes->push_back(colShape);

	//colShape->setMargin(btScalar(0.03));

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(
					btScalar(position.x),
					btScalar(position.y),
					btScalar(position.z)));

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
	if (identifier == "Bullet" || identifier == "Hook") body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	dynamicsWorld->addRigidBody(body);
	physObjects->push_front(this);
	
	body->setGravity(btVector3(0, gravity, 0));
	body->activate(true);
	body->setLinearVelocity(btVector3(cos(rotation) * speed, -sin(rotation) * speed, 0));

	//body->setActivationState(ISLAND_SLEEPING);

	return 0;
}

void RigidObject::setMass(float x) { mass = x; }
void RigidObject::setSpeed(float x) { speed = x; }
void RigidObject::setGravity(float x) { gravity = x; }

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

int RigidObject::collide(RigidObject * other, const btVector3 * worldPos)
{

	return 0;
}

int RigidObject::applyForce(const btVector3 * vel, float m, const btVector3 * worldPos)
{
	/*{
		btVector3 localForce = *worldPos - body->getCenterOfMassPosition();
		btVector3 force = *vel*m*5;

		body->applyCentralForce(force);
        btTransform rotate_with_body;
        rotate_with_body.setIdentity();
        rotate_with_body.setRotation( body->getCenterOfMassTransform().getRotation() );
        body->applyTorque(rotate_with_body(localForce).cross(force)*body->getAngularFactor());
		body->clearForces();
		return 0;
	}*/
	btVector3 localForce = *worldPos - body->getCenterOfMassPosition();
	btVector3 force = *vel*m*5;
	body->activate(true);
	body->applyForce(force, localForce);
	return 0;
}

const btCollisionObject * RigidObject::getBody() { return body; }

void RigidObject::setStatics(btAlignedObjectArray<btCollisionShape*> * col, btDiscreteDynamicsWorld * world, list<RigidObject *> * p)
{
	collisionShapes = col;
	dynamicsWorld = world;
	physObjects = p;
}