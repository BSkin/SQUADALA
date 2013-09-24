#include "Bullet.h"

Bullet::Bullet(char *fileBase, RigidObject * o, long time) : RigidObject(fileBase), dead(false), lifeSpan(60)
{
	owner = o;
	identifier = "Bullet";
	initTime = time;
}

Bullet::~Bullet()
{
}

int Bullet::initBullet()
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
		

	myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	body = new btRigidBody(rbInfo);

	//body->setActivationState(ISLAND_SLEEPING);
	body->setLinearFactor(btVector3(1,1,0));
	body->setAngularFactor(btVector3(0,0,1));
	body->setFriction(0.5f);
	body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	dynamicsWorld->addRigidBody(body);
	physObjects->push_front(this);
	
	body->setGravity(btVector3(0, gravity, 0));
	body->activate(true);
	body->setLinearVelocity(btVector3(cos(rotation) * speed, -sin(rotation) * speed, 0));

	return 0;
}

int Bullet::update(long time)
{
	if (dead || (time - initTime) > lifeSpan) return -1;
	return 0;
}

int Bullet::collide(RigidObject * other, const btVector3 * worldCollPos)
{
	if (other != owner && other->getIndentifier() != "Bullet") 
	{
		dead = true;
		other->applyForce(&body->getLinearVelocity(), mass, worldCollPos);
	}
		
	return 0;
}

bool Bullet::getDead() { return dead; } 