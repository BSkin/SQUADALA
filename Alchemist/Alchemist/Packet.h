#pragma once

#include "GameObject.h"
#include <list>

using std::list;

#pragma region Network Definitions
#define GO_SIZE 48 //Bytes

#define OFFLINE_STATE 0		//No networking
#define ONLINE_STATE 1		//Able to connect or be connected to
#define HOST_STATE 2		//Hosting a connection, cannot connect
#define CLIENT_WAITING 3	//Connecting to a host, cannot host or connect
#define CLIENT_CONNECTED 4	//Connected to a host, cannot host or connect
#pragma endregion

class Packet
{
public:
	static char * serialize();  //Requires Deletion of pointer

	static void deserialize(char *);
	static int getBufferSize();

	static void setList(list<GameObject *> *);
private:
	static list<GameObject *> *gameObjects;
	
	static GameObject * getGameObject(short ID);
	static bool gameObjectsContains(short ID); 
};