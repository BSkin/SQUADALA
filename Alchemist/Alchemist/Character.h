#pragma once

#include "DestructibleObject.h"

class Character : public DestructibleObject
{
public:
	Character(void);
	Character(char *fileBase);
	Character(short ID);
	~Character(void);
	void moveUp(void);
	void moveDown(void);
	void moveLeft(void);
	void moveRight(void);

	virtual int update(long time);
	virtual int renderFrame(long time);

protected:
	float speed;
	D3DXVECTOR3 direction;
};