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