#include "btRigidBodyEx.h"

btRigidBodyEx::btRigidBodyEx(btRigidBodyEx::btRigidBodyConstructionInfo * ci, GameObject * o) : btRigidBody(*ci)
{
	owner = o;
}

GameObject * btRigidBodyEx::getOwner() { return owner; }