#include "Character.h"

Character::Character(void) : RigidObject(), speed(5), direction(0,0,0)
{
	
}

Character::Character(char *fileBase) : RigidObject(fileBase), speed(5), direction(0,0,0)
{
	position = D3DXVECTOR3(0,0,0);
}

Character::Character(short ID) : RigidObject(ID), speed(5), direction(0,0,0)
{
	
}

Character::~Character(void)
{
	
}

int Character::update(long time)
{
	return RigidObject::update(time);
}

int Character::renderFrame(long time)
{
	RigidObject::renderFrame(time);
	return 0;
}

void Character::moveUp()
{
	direction.y += 1;
}

void Character::moveDown()
{
	direction.y -= 1;
}

void Character::moveLeft()
{
	direction.x -= 1;
}

void Character::moveRight()
{
	direction.x += 1;
}