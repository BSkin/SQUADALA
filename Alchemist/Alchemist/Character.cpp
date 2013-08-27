#include "Character.h"

Character::Character(void) : GameObject(), speed(5), direction(0,0,0)
{
	
}

Character::Character(char *fileBase) : GameObject(fileBase), speed(5), direction(0,0,0)
{
	position = D3DXVECTOR3(0,0,0);
}

Character::Character(short ID) : GameObject(ID), speed(5), direction(0,0,0)
{
	
}

Character::~Character(void)
{
	
}

int Character::update(long time)
{
	float finalSpeed = speed;
	if (abs(direction.x) + abs(direction.y) > 1)
	{
		speed *= 0.70710678;
	}
	position += speed * direction;
	return 0;
}

int Character::renderFrame(long time)
{

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