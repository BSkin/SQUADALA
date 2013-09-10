#include "BodyPart.h"

BodyPart::BodyPart(float w, float h, const char *fileBase, int numRows, int numCols) : GameObject(fileBase)
{
	directory = fileBase;
	width = w;
	height = h;
	numSpriteRows = numRows;
	numSpriteCols = numCols;

	curSpriteRow = 0;
	curSpriteCol = 1;
}

int BodyPart::render(D3DXVECTOR3 position, float rotation, bool flipSprite, long time)
{
	if (actorShader == 0 || texture == NULL)
		initGeom();
	else
	{
		this->position = position;
		this->rotation = rotation;
		this->flipSprite = flipSprite;

		//renderPosition = position - D3DXVECTOR3(camera->getPosition().x, camera->getPosition().y, 0);
		
		D3DXMATRIX orient(	-1,			0,			0,			0.0f,
							0,			1,			0,			0.0f,
							0,			0,			1,			0.0f,
							position.x,	position.y,	position.z,	1.0f);

		D3DXMATRIX scale;
		D3DXMatrixScaling(&scale, width, height, 1);
		
		D3DXMATRIX rot;
		D3DXMatrixRotationZ(&rot, rotation);
		
		D3DXMATRIX trans = scale * rot * orient * *viewMatrix * *projMatrix;

		actorShader->SetMatrix(hMatrix, &trans);
		actorShader->SetTexture(hTexture, texture);

		actorShader->SetFloat(hNumSpriteCols, numSpriteCols);
		actorShader->SetFloat(hNumSpriteRows, numSpriteRows);
		actorShader->SetFloat(hCurSpriteCol, curSpriteCol);
		actorShader->SetFloat(hCurSpriteRow, curSpriteRow);
		actorShader->SetBool(hFlipSprite, flipSprite);

		unsigned int numPasses = 0;

		actorShader->Begin(&numPasses, 0);
		for (int i = 0; i < numPasses; i++)
		{
			actorShader->BeginPass(i);
			quad.render(time);
			actorShader->EndPass();
		}
		actorShader->End();
	}

	return 0;
}