#ifndef WEAPON_H
#define WEAPON_H

#include "Human.h"

class Weapon
{
public:
	Weapon(Human *);
	virtual ~Weapon();

	virtual void shoot(long time);
	virtual void changeWeapon(short id);

private:
	Human * owner;

	short currentWeapon;
	long lastFired;
	float invFireRate;
	float bulletDamage, bulletMass, bulletSpeed;
	float bulletWidth, bulletHeight;
};

#endif