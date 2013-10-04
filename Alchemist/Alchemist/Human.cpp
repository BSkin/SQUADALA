#include "Human.h"

Human::Human(void) : Character("Assets\\Player\\Leg Sprites.png"), torso(NULL), head(NULL), frontArms(NULL), backArms(NULL), onGround(true), 
							weapon(WEP_HANDGUN), aimPos(0,0,0)
{
	directory = "Assets\\Player\\Leg Sprites.png";
}

Human::Human(char * path) : Character(path), torso(NULL), head(NULL), frontArms(NULL), backArms(NULL), onGround(true), 
							weapon(WEP_HANDGUN), aimPos(0,0,0)
{
	directory = path;
}

Human::Human(short ID) : Character(ID), torso(NULL), head(NULL), frontArms(NULL), backArms(NULL), onGround(true), 
							weapon(WEP_HANDGUN), aimPos(0,0,0)
{
	
}

Human::~Human(void)
{
	delete torso;
	delete head;
	delete frontArms;
	delete backArms;
}

int Human::initGeom()
{
	RigidObject::initGeom();
	if (torso == NULL)		torso = new		BodyPart(width*0.146484375, height*0.68359375,	"Assets\\Player\\Body Sprite.png",		1, 1);
	if (head == NULL)		head = new		BodyPart(width*0.126953125,	height*0.25390625,	"Assets\\Player\\Head Sprite.png",		1, 1);
	if (frontArms == NULL)	frontArms = new BodyPart(width*1.46484375,	height*0.29296875,	"Assets\\Player\\Arm Front Sprite.png",	3, 1);
	if (backArms == NULL)	backArms = new	BodyPart(width*1.46484375,	height*0.29296875,	"Assets\\Player\\Arm Back Sprite.png",	3, 1);

	numSpriteCols = 7;
	numSpriteRows = 3;
	return 0;
}

int Human::initBullet()
{
	colShape = new btBoxShape(btVector3(width*0.25,height*0.5, 10));
	//colShape = new btSphereShape(btScalar(0.1));

	collisionShapes->push_back(colShape);

	//colShape->setMargin(btScalar(0.03));

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(
					btScalar(position.x),
					btScalar(position.y),
					btScalar(position.z)));

	startTransform.setRotation(btQuaternion(btVector3(0,1,0), 3.14159265358979323846264338327950288419716939));
	startTransform.setRotation(btQuaternion(3.14159265358979323846264338327950288419716939, 0, rotation));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
		

	myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	body = new btRigidBodyEx(&rbInfo, this);

	//body->setActivationState(ISLAND_SLEEPING);
	body->setLinearFactor(btVector3(1,1,0));
	body->setAngularFactor(btVector3(0,0,1));
	body->setFriction(0.5f);
	//body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	dynamicsWorld->addRigidBody(body);
	
	body->setGravity(btVector3(0, gravity, 0));
	body->activate(true);
	//body->setLinearVelocity(btVector3(cos(rotation) * speed, -sin(rotation) * speed, 0));

	//body->setActivationState(ISLAND_SLEEPING);

	return 0;
}

int Human::update(long time)
{
	//direction = D3DXVECTOR3(0,0,0);	

	frontArms->setPosition(position.x*100, position.y*100, renderPosition.z - 0.4);
	frontArms->modPosition(D3DXVECTOR3(	-cos(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425,
										sin(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425, 0));

	if (flipSprite) frontArms->modRotation(-3.14159);
	if (frontArms->getRotation() < 3.14159) frontArms->modRotation(3.14159*2.0); 

	/*
	if (inputManager->keyDown('W')) moveUp();
	if (inputManager->keyDown('S')) moveDown();
	if (inputManager->keyDown('D')) moveRight();
	if (inputManager->keyDown('A')) moveLeft();

	if (inputManager->keyPress('1')) 
	{
		frontArms->setCurRow(0);
		backArms->setCurRow(0);
		weapon = WEP_HANDGUN;
	}
	else if (inputManager->keyPress('2')) 
	{
		frontArms->setCurRow(1);
		backArms->setCurRow(1);
		weapon = WEP_RIFLE;
	}
	else if (inputManager->keyPress('3'))
	{
		frontArms->setCurRow(2);
		backArms->setCurRow(2);
		weapon = WEP_SHOTGUN;
	}

	if (weapon == WEP_HANDGUN)
	{
		if (inputManager->getMouseKeyPress(0))
		{
			float bulletDist = frontArms->getWidth()*0.5+25;
			RigidObject * bullet = new Bullet("Assets\\Player\\Bullet.png", this, time);
			bullet->setPosition(frontArms->getPosition().x*100.0,// + cos(frontArms->getRotation())*bulletDist, 
								frontArms->getPosition().y*100.0,// - sin(frontArms->getRotation())*bulletDist, 
								-1);
			bullet->setMass(0.1);
			bullet->setSize(50, 5);
			bullet->setRotation(frontArms->getRotation());
			bullet->setSpeed(20);
			bullet->setGravity(0);
			//projectileManager->addObject(bullet);
			objects->push_front(bullet);
		}
	}
	else if (weapon == WEP_RIFLE)
	{
		if (inputManager->getMouseKeyDown(0))
		{
			float bulletDist = frontArms->getWidth()*0.5+25;
			RigidObject * bullet = new Bullet("Assets\\Player\\Bullet.png", this, time);
			bullet->setPosition(frontArms->getPosition().x*100.0,// + cos(frontArms->getRotation())*bulletDist, 
								frontArms->getPosition().y*100.0,// - sin(frontArms->getRotation())*bulletDist, 
								-1);
			bullet->setMass(0.01);
			bullet->setSize(50, 5);
			bullet->setRotation(frontArms->getRotation());
			bullet->setSpeed(20);
			bullet->setGravity(0);
			//->addObject(bullet);
			objects->push_front(bullet);
		}
	}
	else if (weapon == WEP_SHOTGUN)
	{
		if (inputManager->getMouseKeyPress(0))
		{
			float r;
			for (int x = 0; x < 4; x++)
			{
				r = rand() % 100;
				r *= 0.005;
				r -= 0.25;
				float bulletDist = frontArms->getWidth()*0.5+25;
				RigidObject * bullet = new Bullet("Assets\\Player\\Bullet.png", this, time);
				bullet->setPosition(frontArms->getPosition().x*100.0 + cos(frontArms->getRotation())*bulletDist, 
									frontArms->getPosition().y*100.0 - sin(frontArms->getRotation())*bulletDist, 
									-1);
				bullet->setMass(0.1);
				bullet->setSize(50, 5);
				bullet->setRotation(frontArms->getRotation() + r);
				bullet->setSpeed(20);
				bullet->setGravity(0);
				//projectileManager->addObject(bullet);
				objects->push_front(bullet);
			}
		}	
	}

	if (inputManager->keyPress('F'))
	{
		hook.shoot(frontArms->getPosition(), frontArms->getRotation());
	}
	*/

	if (onGround && abs(body->getLinearVelocity().y()) < 0.5) 
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
	else
		{ curSpriteRow = 2; curSpriteCol = 0; }

	velocity.x = speed*0.01 * direction.x;

	body->translate(btVector3(velocity.x, velocity.y, velocity.z));
	body->setAngularVelocity(btVector3(0,0,0));
	body->setAngularFactor(0);
	body->activate(true);

	return Character::update(time);
}

int Human::renderFrame(long time)
{
	flipSprite = (aimPos.x+camera->getPosition().x-position.x) < 0;
	Character::renderFrame(time);
	
	torso->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.2);
	head->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.3);
	frontArms->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.4);
	backArms->setPosition(renderPosition.x*100, renderPosition.y*100, renderPosition.z - 0.1);
	
	aimRotation = atan2(aimPos.y + camera->getPosition().y - position.y,
						aimPos.x + camera->getPosition().x - position.x);

	if (flipSprite) aimRotation += 3.14159f;
	if (aimRotation > 3.14159) aimRotation -= 3.14159*2.0;

	torso->setRotation(aimRotation*0.25);
	head->setRotation(aimRotation*0.75);

	head->modPosition(D3DXVECTOR3(	cos(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.5, 
									sin(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.5, 0));

	frontArms->modPosition(D3DXVECTOR3(	cos(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425,
										sin(torso->getRotation()+3.14159*0.5) * torso->getHeight() * 0.425, 0));

	backArms->setPosition(frontArms->getPosition().x*100, frontArms->getPosition().y*100, renderPosition.z - 0.1);
	
	frontArms->setRotation(atan2(	aimPos.y - frontArms->getPosition().y,
									aimPos.x - frontArms->getPosition().x));

	if (flipSprite) frontArms->modRotation(3.14159f);
	if (frontArms->getRotation() > 3.14159) frontArms->modRotation(-3.14159*2.0); 
	
	backArms->render(	backArms->getPosition(),	-frontArms->getRotation(),	flipSprite,	time);
	torso->render(		torso->getPosition(),		-torso->getRotation(),		false,		time);
	head->render(		head->getPosition(),		-head->getRotation(),		flipSprite, time);
	frontArms->render(	frontArms->getPosition(),	-frontArms->getRotation(),	flipSprite, time);

	return 0;
}

void Human::shoot()
{

}

void Human::jump()
{
	if (onGround && abs(body->getLinearVelocity().y()) < 0.5) 
	{
		body->setLinearVelocity(body->getLinearVelocity() + btVector3(0,5,0));
		onGround = false;
	}
}

int Human::collide(GameObject * obj, const btVector3 * worldPos)
{
	if (obj->getIndentifier() != "Bullet") onGround = true;
	return 0;
}