#include "GameObject.h"
#include "stdh.h"

LPDIRECT3DDEVICE9	GameObject::d3dDev =			NULL;
AssetManager *		GameObject::assetManager =		NULL;
Camera *			GameObject::camera =			NULL;
D3DXMATRIX *		GameObject::viewMatrix =		NULL;
D3DXMATRIX *		GameObject::projMatrix =		NULL;
short				GameObject::currentID =			0;

GameObject::GameObject(void) : width(100), height(100), position(0,0,1), renderPosition(0,0,0),
	velocity(0,0,0), acceleration(0,0,0),
	actorShader(0), assetIndex(-1), numSpriteCols(1), numSpriteRows(1), curSpriteCol(0), curSpriteRow(0), flipSprite(FALSE), rotation(0.0f),
	hMatrix(NULL), hTexture(NULL), hNumSpriteCols(NULL), hNumSpriteRows(NULL), hCurSpriteCol(NULL), hCurSpriteRow(NULL), hFlipSprite(NULL), hTechnique(NULL)
{ }

GameObject::GameObject(const char * fileBase, D3DXVECTOR3 pos) : width(100), height(100), position(0,0,1), renderPosition(0,0,0),
	velocity(0,0,0), acceleration(0,0,0),
	actorShader(0), assetIndex(-1), numSpriteCols(1), numSpriteRows(1), curSpriteCol(0), curSpriteRow(0), flipSprite(FALSE), rotation(0.0f),
	hMatrix(NULL), hTexture(NULL), hNumSpriteCols(NULL), hNumSpriteRows(NULL), hCurSpriteCol(NULL), hCurSpriteRow(NULL), hFlipSprite(NULL), hTechnique(NULL)
{
	directory = fileBase;
	position = pos;
	assetIndex = assetManager->getAssetIndex(directory.c_str());
	offset = (rand() % 100);
	ID = currentID++;
}

GameObject::GameObject(short id, D3DXVECTOR3 pos) : width(100), height(100), position(0,0,1), renderPosition(0,0,0),
	velocity(0,0,0), acceleration(0,0,0),
	actorShader(0), assetIndex(-1), numSpriteCols(1), numSpriteRows(1), curSpriteCol(0), curSpriteRow(0), flipSprite(FALSE), rotation(0.0f),
	hMatrix(NULL), hTexture(NULL), hNumSpriteCols(NULL), hNumSpriteRows(NULL), hCurSpriteCol(NULL), hCurSpriteRow(NULL), hFlipSprite(NULL), hTechnique(NULL)
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
	if (quad.texture == NULL || quad.vertexBuffer == NULL)
		quad.init(directory.c_str());
	
	string s = directory;
	s.pop_back();
	s += "png";
	
	if (actorShader == 0)
		assetManager->getEffect("Actor.hlsl", &actorShader);

	if (actorShader != 0)
	{
		hWorld = actorShader->GetParameterByName(NULL, "worldMatrix");
		hMatrix = actorShader->GetParameterByName(NULL, "worldViewProj");
		hTexture = actorShader->GetParameterByName(NULL, "tex");
		hNumSpriteRows = actorShader->GetParameterByName(NULL, "numSpriteRows");
		hNumSpriteCols = actorShader->GetParameterByName(NULL, "numSpriteCols");
		hCurSpriteRow = actorShader->GetParameterByName(NULL, "curSpriteRow");
		hCurSpriteCol = actorShader->GetParameterByName(NULL, "curSpriteCol");
		hFlipSprite = actorShader->GetParameterByName(NULL, "flipSprite");
		hTechnique = actorShader->GetTechniqueByName("ActorTechnique");

		actorShader->SetTechnique(hTechnique);
	}

	return 0;
}

/*
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
*/

int GameObject::update(long time)
{
	/*
	if (!physInit)
	{
		initBullet();
		physInit = true;
	}
	*/
	/*
	//setAcceleration(0, -0.16f, 0);

	//position.y = sin(time*0.01+offset)*10;
	//velocity += acceleration;
	//position += velocity;

	float y = 0;//ocean->getHeight(position, time);

	if (position.y <= y)
	{
		position.y = y;
		setVelocity(0,0,0);
	}*/
	/*
	#pragma region Update Transformation Matrix

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
	*/
	return 0;
}

int GameObject::renderFrame(long time)
{	
	if (actorShader == 0)
		initGeom();
	else
	{
		D3DXVECTOR3 left, up, forward;
		left = D3DXVECTOR3(-1,0,0);
		up = D3DXVECTOR3(0,1,0);
		forward = D3DXVECTOR3(0,0,1);

		renderPosition = position - D3DXVECTOR3(camera->getPosition().x, camera->getPosition().y, 0);
		
		D3DXMATRIX orient(	left.x,				left.y,				left.z,				0.0f,
							up.x,				up.y,				up.z,				0.0f,
							forward.x,			forward.y,			forward.z,			0.0f,
							renderPosition.x,	renderPosition.y,	renderPosition.z,	1.0f);

		D3DXMATRIX scale;
		D3DXMatrixScaling(&scale, width, height, 1);
		
		D3DXMATRIX rot;
		D3DXMatrixRotationZ(&rot, rotation);
		
		D3DXMATRIX trans = scale * rot * orient * *viewMatrix * *projMatrix;

		actorShader->SetMatrix(hMatrix, &trans);
		actorShader->SetTexture(hTexture, quad.texture);

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

int GameObject::renderFrame(long time, D3DXMATRIX * trans)
{
	if (actorShader == 0)
		initGeom();
	else
	{
		D3DXMATRIX camTrans;// - D3DXVECTOR3(camera->getPosition().x, camera->getPosition().y, 0);
		D3DXMatrixTranslation(&camTrans, -camera->getPosition().x, -camera->getPosition().y,0);

		D3DXMATRIX scale;
		D3DXMatrixScaling(&scale, width, height, 0);
		D3DXMATRIX asdf = scale * *trans * camTrans * *viewMatrix * *projMatrix;

		actorShader->SetMatrix(hMatrix, &asdf);
		actorShader->SetTexture(hTexture, quad.texture);

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

D3DXVECTOR3 GameObject::getPosition() { return position; }

void GameObject::modVelocity(D3DXVECTOR3 x) { velocity += x; }
void GameObject::modVelocity(float x, float y, float z) { modVelocity(D3DXVECTOR3(x,y,z)); }

#pragma region Set Functions
void GameObject::setPosition(D3DXVECTOR3 pos) { position = pos*0.01f; }
void GameObject::setPosition(float x, float y, float z) { position = D3DXVECTOR3(x*0.01f, y*0.01f, z); }
void GameObject::setPositionY(float y) { position.y = y*0.01f; }
void GameObject::setSize(int x, int y) { width = x*0.01f; height = y*0.01f; quad.setSize(width, height);}
void GameObject::setVelocity(D3DXVECTOR3 vel) { velocity = vel; }
void GameObject::setVelocity(float x, float y, float z) { velocity = D3DXVECTOR3(x,y,z); }
void GameObject::setAcceleration(D3DXVECTOR3 accel) { acceleration = accel; }
void GameObject::setAcceleration(float x, float y, float z) { acceleration = D3DXVECTOR3(x,y,z); }
void GameObject::setRotation(float x) { rotation = x; }
void GameObject::modRotation(float x) { rotation += x; }
void GameObject::setID(short x) { ID = x; }
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

int GameObject::setCamera(Camera * cam)
{
	camera = cam;
	return 0;
}

void GameObject::setViewMatrix(D3DXMATRIX * matrix) { viewMatrix = matrix; }
void GameObject::setProjMatrix(D3DXMATRIX * matrix) { projMatrix = matrix; }
void GameObject::setMatrices(D3DXMATRIX * view, D3DXMATRIX * proj)
{
	setViewMatrix(view);
	setProjMatrix(proj);
}
#pragma endregion