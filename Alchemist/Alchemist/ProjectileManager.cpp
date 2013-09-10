#include "ProjectileManager.h"

ProjectileManager::ProjectileManager() {}

void ProjectileManager::addObject(RigidObject * r)
{
	physList.push_front(r);
}

int ProjectileManager::render(long time)
{
	list<RigidObject *>::iterator iter;
	for (iter = physList.begin(); iter != physList.end(); iter++)
	{
		(*iter)->update(time);
		(*iter)->renderFrame(time);
	}
	return 0;
}

void ProjectileManager::clearList() 
{
	list<RigidObject *>::iterator iter;
	for (iter = physList.begin(); iter != physList.end(); iter++)
	{
		if (*iter)
		{
			delete *iter;
			*iter = 0;
		}
	}
	physList.clear();
}