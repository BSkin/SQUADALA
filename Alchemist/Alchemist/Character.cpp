#include "Character.h"

Character::Character(void) : DestructibleObject(), speed(5), direction(0,0,0)
{
	
}

Character::Character(char *fileBase) : DestructibleObject(fileBase), speed(5), direction(0,0,0)
{
	position = D3DXVECTOR3(0,0,0);
}

Character::Character(short ID) : DestructibleObject(ID), speed(5), direction(0,0,0)
{
	
}

Character::~Character(void)
{
	
}

int Character::update(long time)
{
	return DestructibleObject::update(time);
}

int Character::renderFrame(long time)
{
	DestructibleObject::renderFrame(time);
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