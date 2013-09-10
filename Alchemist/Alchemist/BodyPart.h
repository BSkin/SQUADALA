#pragma once

#include "GameObject.h"

class BodyPart : public GameObject
{
public:
	BodyPart(float width, float height, const char *fileBase, int numRows, int numCols);
	
	int render(D3DXVECTOR3 position, float rotation, bool flipSprite, long time);

private:

};
