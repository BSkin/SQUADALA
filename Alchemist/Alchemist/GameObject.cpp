#include "GameObject.h"
#include "stdh.h"

LPDIRECT3DDEVICE9	GameObject::d3dDev =			NULL;
AssetManager *		GameObject::assetManager =		NULL;
D3DXMATRIX *		GameObject::viewMatrix =		NULL;
D3DXMATRIX *		GameObject::projMatrix =		NULL;
D3DXVECTOR3			GameObject::cameraPosition =	D3DXVECTOR3(0,0,0);
short				GameObject::currentID =			0;
D3DXVECTOR3 *		GameObject::sunDir =			NULL;

btAlignedObjectArray<btCollisionShape*> *	GameObject::collisionShapes =	NULL;
btDiscreteDynamicsWorld *					GameObject::dynamicsWorld =		NULL;


GameObject::GameObject(void) : scale(1.0f,1.0f,1.0f), forwardVector(0,0,1), upVector(0,1,0), sideVector(1,0,0), position(0,0,0), collisionRadius(0), 
	horizontalRotation(0.0f), verticalRotation(0.0f), velocity(0,0,0), acceleration(0,0,0),
	actorShader(0), texture(0), mesh(0), animated(false), animationIndex(-1), assetIndex(-1), physInit(false)
{ }

GameObject::GameObject(char * fileBase, D3DXVECTOR3 pos) : scale(1.0f,1.0f,1.0f), forwardVector(0,0,1), upVector(0,1,0), sideVector(1,0,0), position(0,0,0), collisionRadius(0), 
	horizontalRotation(0.0f), verticalRotation(0.0f), velocity(0,0,0), acceleration(0,0,0),
	actorShader(0), texture(0), mesh(0), animated(false), animationIndex(-1), assetIndex(-1), physInit(false)
{
	directory = fileBase;
	position = pos;
	assetIndex = assetManager->getAssetIndex(directory.c_str());
	offset = (rand() % 100);
	ID = currentID++;
}

GameObject::GameObject(short id, D3DXVECTOR3 pos) : scale(1.0f,1.0f,1.0f), forwardVector(0,0,1), upVector(0,1,0), sideVector(1,0,0), position(0,0,0), collisionRadius(0), 
	horizontalRotation(0.0f), verticalRotation(0.0f), velocity(0,0,0), acceleration(0,0,0),
	actorShader(0), texture(0), mesh(0), animated(false), animationIndex(-1), assetIndex(-1), physInit(false)
{
	directory = assetManager->getAssetPath(id);
	position = pos;
	assetIndex = id;
	offset = (rand() % 100);
	ID = currentID++;
}

GameObject::~GameObject()
{

}

int GameObject::initGeom()
{
	if (mesh == 0 && skinnedData.isNull())
		assetManager->getModel(directory.c_str(), &mesh, &skinnedData);
	
	if (skinnedData.isNull()) animated = true;
	else if (mesh) animated = false;

	//if (!mesh) skinnedData.SetAnimationSet(2);

	string s = directory;
	s.pop_back();
	s += "png";

	if (texture == 0)
		assetManager->getTexture(s.c_str(), &texture);
	
	if (actorShader == 0)
		assetManager->getEffect("Actor.hlsl", &actorShader);

	if (actorShader != 0)
	{
		hWorld = actorShader->GetParameterByName(NULL, "worldMatrix");
		hMatrix = actorShader->GetParameterByName(NULL, "worldViewProj");
		hTexture = actorShader->GetParameterByName(NULL, "tex");
		hTechnique = actorShader->GetTechniqueByName("ActorTechnique");
		hFinalTransforms = actorShader->GetParameterByName(NULL, "finalTransforms");
		hAnimated = actorShader->GetParameterByName(NULL, "animated");
		hSunDir = actorShader->GetParameterByName(NULL, "sunDir");

		actorShader->SetTechnique(hTechnique);
	}

	return 0;
}

int GameObject::initBullet()
{
	colShape = new btBoxShape(btVector3(scale.x,scale.y,scale.z));
	btStridingMeshInterfaceData asdf;
	//
	//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	collisionShapes->push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.f);
		
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
		
	startTransform.setOrigin(btVector3(
					btScalar(position.x),
					btScalar(position.y),
					btScalar(position.z)));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);
	body->setFriction(0.5f);

	return 0;
}

int GameObject::update(long time)
{
	if (!physInit)
	{
		initBullet();
		physInit = true;
	}

	//setAcceleration(0, -0.16f, 0);

	//position.y = sin(time*0.01+offset)*10;
	//velocity += acceleration;
	//position += velocity;

	/*float y = 0;//ocean->getHeight(position, time);

	if (position.y <= y)
	{
		position.y = y;
		setVelocity(0,0,0);
	}*/

	if (!skinnedData.isNull())
	{
		D3DXMATRIX worldMatrix, scaleMatrix, translationMatrix, rotationMatrix;

		D3DXMatrixIdentity(&worldMatrix);
		D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
		D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
		D3DXMatrixRotationY(&rotationMatrix, horizontalRotation);

		worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

		skinnedData.FrameMove(1.0f/60.0f, &worldMatrix);
	}

	#pragma region Update Transformation Matrix
	/*D3DXMATRIX scaleMatrix, translationMatrix, rotationMatrix;
	
	D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
	D3DXMatrixRotationY(&rotationMatrix, horizontalRotation);
	
	worldMatrix *= scaleMatrix;
	worldMatrix *= rotationMatrix;
	worldMatrix *= translationMatrix;
	*/
	body->translate(btVector3(velocity.x, velocity.y, velocity.z));

	btTransform * bulletTransformMatrix = &body->getWorldTransform();
	btVector3 R = bulletTransformMatrix->getBasis().getColumn(0);
	btVector3 U = bulletTransformMatrix->getBasis().getColumn(1);
	btVector3 L = bulletTransformMatrix->getBasis().getColumn(2);
	btVector3 P = bulletTransformMatrix->getOrigin();
				
	D3DXVECTOR3 vR, vU, vL, vP;
	vR.x = R.x();vR.y = R.y();vR.z = R.z();
	vU.x = U.x();vU.y = U.y();vU.z = U.z();
	vL.x = L.x();vL.y = L.y();vL.z = L.z();
	vP.x = P.x();vP.y = P.y();vP.z = P.z();

	D3DXMATRIX matOutput;
	matOutput._11 = vR.x;matOutput._12 = vR.y;matOutput._13 = vR.z;matOutput._14 = 0.f;
	matOutput._21 = vU.x;matOutput._22 = vU.y;matOutput._23 = vU.z;matOutput._24 = 0.f;
	matOutput._31 = vL.x;matOutput._32 = vL.y;matOutput._33 = vL.z;matOutput._34 = 0.f;
	matOutput._41 = vP.x;matOutput._42 = vP.y;matOutput._43 = vP.z;matOutput._44 = 1.f;

	D3DXMATRIX scaleMatrix; D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
	worldMatrix = scaleMatrix * matOutput;
	#pragma endregion

	return 0;
}

int GameObject::renderFrame(long time)
{
	if (!physInit)
	{
		initBullet();
		physInit = true;
	}
	if ((mesh == 0 && skinnedData.isNull()) || texture == 0 || actorShader == 0) initGeom();
	else
	{	
		worldViewProj = worldMatrix * *viewMatrix * *projMatrix;
	
		//d3dDev->SetTransform(D3DTS_WORLD, &worldMatrix);
	
		//d3dDev->SetTexture(0, texture);

		//D3DXVECTOR3 normal =  cameraPosition - position;
	
		if (actorShader)
		{
			unsigned int numPasses = 0;
	
			#pragma region Set Shader Data

			if (animated) actorShader->SetMatrixArray(hFinalTransforms, finalTransforms, 35);
			actorShader->SetMatrix(hWorld, &worldMatrix);
			actorShader->SetMatrix(hMatrix, &worldViewProj);
			actorShader->SetBool(hAnimated, animated);
			actorShader->SetTexture(hTexture, texture);
			actorShader->SetFloatArray(hSunDir, *sunDir, 3);
	
			#pragma endregion
		
			actorShader->Begin(&numPasses, 0);
		
			for (int i = 0; i < numPasses; i++) 
			{
				actorShader->BeginPass(i);
			
				if (mesh)
					mesh->DrawSubset(i);
				else
				{
					skinnedData.Render();
				}
			
				actorShader->EndPass();
			
			}
	
			actorShader->End();
		}
		else
		{
			if (mesh) mesh->DrawSubset(0);
			if (!skinnedData.isNull()) skinnedData.Render();
		}
	}
	return 0;
}

D3DXVECTOR3 GameObject::getPosition() { return position; }

void GameObject::modVelocity(D3DXVECTOR3 x) { velocity += x; }
void GameObject::modVelocity(float x, float y, float z) { modVelocity(D3DXVECTOR3(x,y,z)); }

#pragma region Set Functions
void GameObject::setPosition(D3DXVECTOR3 pos) { position = pos; }
void GameObject::setPosition(float x, float y, float z) { position = D3DXVECTOR3(x, y, z); }
void GameObject::setPositionY(float y) { position.y = y; }
void GameObject::setScale(D3DXVECTOR3 Scale) { scale = Scale; }
void GameObject::setScale(float x, float y, float z) { scale = D3DXVECTOR3(x,y,z); }
void GameObject::setVelocity(D3DXVECTOR3 vel) { velocity = vel; }
void GameObject::setVelocity(float x, float y, float z) { velocity = D3DXVECTOR3(x,y,z); }
void GameObject::setAcceleration(D3DXVECTOR3 accel) { acceleration = accel; }
void GameObject::setAcceleration(float x, float y, float z) { acceleration = D3DXVECTOR3(x,y,z); }
void GameObject::setRotation(D3DXVECTOR2 target) { horizontalRotation = target.x; verticalRotation = target.y; }
void GameObject::setRotation(float x, float y) { horizontalRotation = x; verticalRotation = y; }
void GameObject::setRotation(float x) { horizontalRotation = x; }
void GameObject::setAnimationIndex(short x) { animationIndex = x; }
void GameObject::setID(short x) { ID = x; }
#pragma endregion

#pragma region Move Functions
void GameObject::moveForward()
{
	D3DXVECTOR3 temp = D3DXVECTOR3(
		sin(horizontalRotation),
		0,
		cos(horizontalRotation));
	velocity -= temp*0.25;
}
void GameObject::moveBackward()
{
	D3DXVECTOR3 temp = D3DXVECTOR3(
		sin(horizontalRotation),
		0,
		cos(horizontalRotation));
	velocity += temp*0.25;
}
void GameObject::moveLeft()
{
	D3DXVECTOR3 temp = D3DXVECTOR3(
		sin(horizontalRotation + 1.5707963267948966192313216916398),
		0,
		cos(horizontalRotation + 1.5707963267948966192313216916398));
	velocity += temp*0.25;
}
void GameObject::moveRight()
{
	D3DXVECTOR3 temp = D3DXVECTOR3(
		sin(horizontalRotation + 1.5707963267948966192313216916398),
		0,
		cos(horizontalRotation + 1.5707963267948966192313216916398));
	velocity -= temp*0.25;
}
#pragma endregion

#pragma region Set Statics
int GameObject::setd3dDev(IDirect3DDevice9 *d3d_Dev)
{
	if (d3dDev == NULL)
		d3dDev = d3d_Dev;
	else
		return 1; //device is already set, cannot be re set

	return 0; //normal exit status
}

int GameObject::setAssetManager(AssetManager *ass)
{
	if (assetManager == NULL)
		assetManager = ass;
	else
		return 1;
	return 0;
}

void GameObject::setBullet(btAlignedObjectArray<btCollisionShape*> * col, btDiscreteDynamicsWorld * world)
{
	collisionShapes = col;
	dynamicsWorld = world;
}

void GameObject::setViewMatrix(D3DXMATRIX * matrix) { viewMatrix = matrix; }
void GameObject::setProjMatrix(D3DXMATRIX * matrix) { projMatrix = matrix; }
void GameObject::setMatrices(D3DXMATRIX * view, D3DXMATRIX * proj)
{
	setViewMatrix(view);
	setProjMatrix(proj);
}
void GameObject::setCameraPosition(D3DXVECTOR3 camPos) { cameraPosition = camPos; }
void GameObject::setSunDir(D3DXVECTOR3 * x) { sunDir = x; }
#pragma endregion