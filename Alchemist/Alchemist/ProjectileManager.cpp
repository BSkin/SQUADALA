#include "ProjectileManager.h"

ProjectileManager::ProjectileManager() : physList(), deadList()
{}

void ProjectileManager::addObject(RigidObject * r)
{
	physList.push_front(r);
}

int ProjectileManager::render(long time)
{
	list<RigidObject *>::iterator iter;
	for (iter = physList.begin(); iter != physList.end(); iter++)
	{
		int x = (*iter)->update(time);
		if (x == -1) 
		{
			deadList.push_front(*iter);
		}
	}

	for (iter = deadList.begin(); iter != deadList.end(); iter++)
	{
		physList.remove(*iter);
		delete *iter;
	}
	deadList.clear();


	for (iter = physList.begin(); iter != physList.end(); iter++)
	{
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

list<RigidObject *> * ProjectileManager::getPhysList()
{
	return &physList;
}

int ProjectileManager::getSize() { return physList.size(); }