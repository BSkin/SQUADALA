#include "Bullet.h"

Bullet::Bullet(char *fileBase, RigidObject * o) : RigidObject(fileBase), dead(false)
{
	owner = o;
	identifier = "Bullet";
}

Bullet::~Bullet()
{
}

int Bullet::update(long time)
{
	if (dead) return -1;
	return 0;
}

int Bullet::collide(RigidObject * other)
{
	if (other != owner && other->getIndentifier() != "Bullet") 
		dead = true;
	return 0;
}

bool Bullet::getDead() { return dead; } 