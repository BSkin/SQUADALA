#ifndef BT_RIGID_BODY_EX
#define BT_RIGID_BODY_EX

#include "Utility.h"
#include "GameObject.h"

class btRigidBodyEx : public btRigidBody
{
public:
	btRigidBodyEx(btRigidBody::btRigidBodyConstructionInfo *, GameObject *);
	GameObject * getOwner();

private:
	GameObject * owner;
};

#endif