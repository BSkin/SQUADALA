#include "GrappleHook.h"

GrappleHook::GrappleHook(Character * o) : RigidObject("Assets\\Player\\Hook.png"), state(IDLE),
								owner(NULL), attatchedObject(NULL), line("Assets\\Player\\Hook Wire.png")
{
	owner = o;
	identifier = "Hook";
	//setSpeed(10);
	setSize(50,50);
	setPosition(owner->getPosition().x*100,owner->getPosition().y*100, owner->getPosition().z);
	setGravity(0);
	/*if (colShape)
	{
		delete colShape;
		colShape = 0;
	}*/
}

GrappleHook::~GrappleHook()
{

}

int GrappleHook::initBullet()
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

int GrappleHook::update(long time)
{
	RigidObject::update(time);

	if (state == LATCHED)
	{
		D3DXVECTOR3 temp = position-owner->getPosition();
		D3DXVec3Normalize(&temp, &temp);
		temp *= 5;
		owner->applyForce(&Utility::ConvertToBt(&temp), 0.1f, &Utility::ConvertToBt(&owner->getPosition()));
	}
	//position = owner->getPosition();
	//if (state == LATCHED) hook->setPosition(attatchedObject->getPosition());
	return 0;
}

int GrappleHook::renderFrame(long time)
{
	if (!physInit) RigidObject::renderFrame(time);
	if (state != IDLE)
	{
		float x = sqrt(
			pow(position.x - owner->getPosition().x,2) + 
			pow(position.y - owner->getPosition().y,2));
		line.setSize(x*100, 10);

		float rot = atan2(
						-(owner->getPosition().y - position.y),
						owner->getPosition().x - position.x);
		line.setRotation(rot);

		D3DXVECTOR3 pos = D3DXVECTOR3(
									(owner->getPosition().x + position.x) * 0.5,
									(owner->getPosition().y + position.y) * 0.5,
									owner->getPosition().z);
		line.setPosition(pos*100);

		line.renderFrame(time);
		RigidObject::renderFrame(time);
	}
	return 0;
}

int GrappleHook::collide(GameObject * other, const btVector3 * worldPos)
{
	if (other == owner && state == LATCHED)
	{
		state = IDLE;
	}
	else if (other != owner && other->getIndentifier() != "Bullet" && other->getIndentifier() != "Hook")
	{
		if (state != IDLE)
		{
			body->setLinearVelocity(btVector3(0,0,0));
			if (state == EXTENDING) latch(other);
		}
	}
	return 0;
}

void GrappleHook::shoot(D3DXVECTOR3 pos, float rot)
{
	if (state == IDLE)
	{
		body->activate(true);
		setSpeed(10);
		setSize(50,50);
		setPosition(pos.x*100,pos.y*100, pos.z);
		setRotation(rot);

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(
					btScalar(pos.x),
					btScalar(pos.y),
					btScalar(pos.z)));

		startTransform.setRotation(btQuaternion(3.14159265358979323846264338327950288419716939, 0, rotation));

		body->setWorldTransform(startTransform);
		body->setLinearVelocity(btVector3(cos(rotation) * speed, -sin(rotation) * speed, 0));
		body->setGravity(btVector3(0,-9.8,0));

		state = EXTENDING;
	}
	else
	{
		unlatch();
		body->setLinearVelocity(btVector3(0, 0, 0));
		body->setGravity(btVector3(0,0,0));
		//body->activate(true);
	}
}

void GrappleHook::latch(GameObject * o)
{
	//if (state == EXTENDING)
	//{
		//o->getBody()->getWorldTransform().getRotation();
		attatchedObject = o;
		body->setLinearVelocity(btVector3(0, 0, 0));
		body->setGravity(btVector3(0,0,0));
		state = LATCHED;
	//}
}

void GrappleHook::unlatch()
{
		attatchedObject = 0;
		state = IDLE;
}