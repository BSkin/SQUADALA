#pragma once

#include "RigidObject.h"
#include <list>

using std::list;

class ProjectileManager
{
public:
	ProjectileManager();
	void addObject(RigidObject *);
	void clearList();
	int render(long time);

private:
	list<RigidObject *> physList;
};