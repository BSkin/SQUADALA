#include "Player.h"

RawInputManager *	Player::inputManager =	NULL;
double		 		Player::cursorX =		0.0;
double				Player::cursorY =		0.0;

Player::Player(void) : Character("Assets\\Player\\Leg Sprites.png"), torso(NULL), head(NULL), frontArms(NULL), backArms(NULL), crosshair(NULL)
{
	directory = "Assets\\Player\\Leg Sprites.png";
}

Player::Player(short ID) : Character(ID), torso(NULL), head(NULL), frontArms(NULL), backArms(NULL), crosshair(NULL)
{
	
}

Player::~Player(void)
{
	delete torso;
	delete head;
	delete frontArms;
	delete backArms;
}

int Player::initGeom()
{
	if (torso == NULL)		torso = new		BodyPart(width*0.146484375, height*0.68359375,	"Assets\\Player\\Body Sprite.png",		1, 1);
	if (head == NULL)		head = new		BodyPart(width*0.126953125,	height*0.25390625,	"Assets\\Player\\Head Sprite.png",		1, 1);
	if (frontArms == NULL)	frontArms = new BodyPart(width*1.46484375,	height*0.29296875,	"Assets\\Player\\Arm Front Sprite.png",	2, 1);
	if (backArms == NULL)	backArms = new	BodyPart(width*1.46484375,	height*0.29296875,	"Assets\\Player\\Arm Back Sprite.png",	2, 1);
	if (crosshair == NULL)	crosshair = new	BodyPart(0.5,				0.5,				"Assets\\Player\\Crosshair.png",		1, 1);

	GameObject::initGeom();
	numSpriteCols = 7;
	numSpriteRows = 3;
	return 0;
}

int Player::update(long time)
{
	direction = D3DXVECTOR3(0,0,0);	

	if (inputManager->keyDown('W')) moveUp();
	if (inputManager->keyDown('S')) moveDown();
	if (inputManager->keyDown('D')) moveRight();
	if (inputManager->keyDown('A')) moveLeft();

	if (inputManager->getMouseKeyPress(0))
	{
		float bulletDist = height*42.5+25;
		RigidObject * bullet = new Bullet("Assets\\Player\\Bullet.png", this);
		bullet->setPosition(frontArms->getPosition().x*100 + cos(frontArms->getRotation())*bulletDist, 
							frontArms->getPosition().y*100 - sin(frontArms->getRotation())*bulletDist, 
							position.z);
		bullet->setMass(0.1);
		bullet->setSize(50, 5);
		bullet->setRotation(frontArms->getRotation());
		bullet->setSpeed(30);
		projectileManager->addObject(bullet);
	}
	if (inputManager->getMouseKeyDown(1) && time % 2 > 0)
	{
		RigidObject * bullet = new Bullet("Assets\\Player\\Bullet.png", this);
		bullet->setPosition(frontArms->getPosition().x*100 + cos(frontArms->getRotation())*100, 
							frontArms->getPosition().y*100 - sin(frontArms->getRotation())*100, 
							position.z);
		bullet->setMass(0.1);
		bullet->setSize(55, 5);
		bullet->setRotation(frontArms->getRotation());
		bullet->setSpeed(30);
		projectileManager->addObject(bullet);
	}

	if (inputManager->keyPress('1')) 
	{
		frontArms->setCurRow(0);
		backArms->setCurCol(0);
	}
	else if (inputManager->keyPress('2')) 
	{
		frontArms->setCurRow(1);
		backArms->setCurCol(1);
	}

	if (abs(body->getLinearVelocity().y()) > 0.5) 
		{ curSpriteRow = 2; curSpriteCol = 0; }
	else
	{
		if (direction.x) 
		{ 
				curSpriteRow = 0; 
				if (flipSprite) curSpriteCol = (int)time*(-direction.x)*0.125;
				else curSpriteCol = (int)time*direction.x*0.125; 
		}
		else 
			{ curSpriteRow = 1; curSpriteCol = 0; }
	}

	velocity.x = speed*0.01 * direction.x;
	if (inputManager->keyPress(VK_SPACE)) body->setLinearVelocity(body->getLinearVelocity() + btVector3(0,5,0));

	body->translate(btVector3(velocity.x, velocity.y, velocity.z));
	body->setAngularVelocity(btVector3(0,0,0));
	body->setAngularFactor(0);
	body->activate(true);

	return Character::update(time);
}

int Player::renderFrame(long time)
{
	flipSprite = (cursorX+camera->getPosition().x-position.x) < 0;
	Character::renderFrame(time);
	
	torso->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.2);
	head->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.3);
	frontArms->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.4);
	backArms->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.1);
	crosshair->setPosition(cursorX*100, cursorY*100, renderPosition.z - 0.5);
	
	aimRotation = atan2(cursorY + camera->getPosition().y - position.y,
						cursorX + camera->getPosition().x - position.x);

	if (flipSprite) aimRotation += 3.14159f;
	if (aimRotation > 3.14159) aimRotation -= 3.14159*2.0;

	torso->setRotation(aimRotation*0.25);
	head->setRotation(aimRotation*0.75);

	head->modPosition(D3DXVECTOR3(	cos(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.5, 
									sin(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.5, 0));

	frontArms->modPosition(D3DXVECTOR3(	cos(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425,
										sin(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425, 0));

	backArms->setPosition(frontArms->getPosition().x*100, frontArms->getPosition().y*100, renderPosition.z - 0.1);
	
	frontArms->setRotation(atan2(	cursorY - frontArms->getPosition().y,
									cursorX - frontArms->getPosition().x));
	if (flipSprite) frontArms->modRotation(3.14159f);
	if (frontArms->getRotation() > 3.14159) frontArms->modRotation(-3.14159*2.0); 
	
	backArms->render(	backArms->getPosition(),	-frontArms->getRotation(),	flipSprite,	time);
	torso->render(		torso->getPosition(),		-torso->getRotation(),		false,		time);
	head->render(		head->getPosition(),		-head->getRotation(),		flipSprite, time);
	frontArms->render(	frontArms->getPosition(),	-frontArms->getRotation(),	flipSprite, time);
	crosshair->render(	crosshair->getPosition(),	0.0,						false,		time);

	frontArms->setPosition(position.x*100, position.y*100, renderPosition.z - 0.4);
	frontArms->modPosition(D3DXVECTOR3(	cos(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425,
										sin(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425, 0));

	if (flipSprite) frontArms->modRotation(-3.14159);
	if (frontArms->getRotation() < 3.14159) frontArms->modRotation(3.14159*2.0); 

	return 0;
}

int Player::setInputManager(RawInputManager * iManager)
{
	if (iManager) 
	{
		inputManager = iManager;
		return 0;
	}
	return -1;
}

void Player::setCursorPos(double x, double y) { cursorX = x; cursorY = y;}