#include "Weapon.h"

Weapon::Weapon(Human * h) : bulletDamage(5.0), bulletMass(0.1), bulletWidth(50.0), bulletHeight(5.0), bulletSpeed(20.0)
{
	lastFired = 0.0;
	owner = h;
}

void Weapon::shoot(long time)
{
	if ((lastFired + invFireRate) <= time)
	{
		lastFired = time;
	}
}

void Weapon::changeWeapon(short id)
{
	bulletDamage = 5.0;
	bulletMass = 0.1;
	bulletWidth = 50.0;
	bulletHeight = 5.0;
	bulletSpeed = 20.0;
}