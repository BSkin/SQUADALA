#include "Bullet.h"

Bullet::Bullet(char *fileBase, RigidObject * o, long time) : RigidObject(fileBase), damage(5.0f)
{
	owner = o;
	identifier = "Bullet";
	health = 120;
}

Bullet::~Bullet()
{
	int x = 2;
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
	btRigidBodyEx::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	body = new btRigidBodyEx(&rbInfo, this);

	//body->setActivationState(ISLAND_SLEEPING);
	body->setLinearFactor(btVector3(1,1,0));
	body->setAngularFactor(btVector3(0,0,1));
	body->setFriction(0.5f);
	body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	dynamicsWorld->addRigidBody(body);
	
	body->setGravity(btVector3(0, gravity, 0));
	body->activate(true);
	body->setLinearVelocity(btVector3(cos(rotation) * speed, -sin(rotation) * speed, 0));

	return 0;
}

int Bullet::update(long time)
{
	health -= 1;
	return 0;
}

int Bullet::collide(GameObject * other, const btVector3 * worldCollPos)
{
	if (health > 0 && other != owner && other->getIndentifier() != "Bullet") 
	{
		health = 0;
		other->applyForce(&body->getLinearVelocity(), mass, worldCollPos);
		other->modHealth(-damage);
	}
		
	return 0;
}

bool Bullet::getDead() { return health <= 0; } 