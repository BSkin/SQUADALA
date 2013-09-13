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

	int update(long time);
	int render(long time);

	list<RigidObject *> * getPhysList();
	int getSize();

private:
	list<RigidObject *> physList;
	list<RigidObject *> deadList;
};