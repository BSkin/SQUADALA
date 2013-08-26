#include "Packet.h"

list<GameObject *> * Packet::gameObjects = NULL;

char * Packet::serialize()
{
	int count = gameObjects->size();
	char * packet = new char[GO_SIZE * count];

	for (int i = 0; i < GO_SIZE * count; i++)
	{
		printf("%c", packet[i]);
	}
	printf("%c", '\n');

	list<GameObject *>::iterator iter;
	int index = 0;

	for (iter = gameObjects->begin(); iter != gameObjects->end(); iter++)
	{
		//char * tempPacket = new char[GO_SIZE]; 
		char header[] = {'-', 'g'};

		memcpy(packet+index*GO_SIZE,		header,					2);

		memcpy(packet+2+index*GO_SIZE,	&(*iter)->position.x,	4);
		memcpy(packet+6+index*GO_SIZE,	&(*iter)->position.y,	4);
		memcpy(packet+10+index*GO_SIZE,	&(*iter)->position.z,	4);
	
		memcpy(packet+14+index*GO_SIZE,	&(*iter)->velocity.x,	4);
		memcpy(packet+18+index*GO_SIZE,	&(*iter)->velocity.y,	4);
		memcpy(packet+22+index*GO_SIZE,	&(*iter)->velocity.z,	4);

		memcpy(packet+26+index*GO_SIZE,	&(*iter)->acceleration.x,4);
		memcpy(packet+30+index*GO_SIZE,	&(*iter)->acceleration.y,4);
		memcpy(packet+34+index*GO_SIZE,	&(*iter)->acceleration.z,4);

		memcpy(packet+38+index*GO_SIZE,	&(*iter)->horizontalRotation,4);

		memcpy(packet+42+index*GO_SIZE,	&(*iter)->animationIndex,2);
	
		memcpy(packet+44+index*GO_SIZE,	&(*iter)->ID,			2);

		memcpy(packet+46+index*GO_SIZE,	&(*iter)->assetIndex,	2);

		index++;
	}

	for (int i = 0; i < GO_SIZE * count; i++)
	{
		if (i < 2) printf("%c", packet[i]);
		else if (i == 2 || i == 6 || i == 10 || i == 14 || i == 18 || i == 22 || i == 26 || i == 30 || i == 34 || i == 38)
		{
			printf("%f", (float)packet[i]);
		}
		//else if (i == 42|| i == 44 || i == 46)
		//{
		//	printf("%s", packet[i]);
		//}
	}
	printf("%c", '\n');

	int x = 0;

	return packet;
}

void Packet::deserialize(char * packet)
{
	int i = 0;
	while ((char)packet[i] == '-' && i < 2000) 
	{
		if ((char)packet[i+1] == 'g')
		{
			short ID;
			memcpy(&ID, &packet[i+44], 4);

			GameObject * temp = getGameObject(ID);
			
			if (temp == NULL) //Add GameObject
			{
				temp = new GameObject((short)packet[i+46]);
				if (temp->assetIndex == 4) temp->setScale(0.1,0.1,0.1);
				memcpy(&temp->ID, &packet[i+44], 4);
				gameObjects->push_back(temp);
			}
			
			memcpy(&temp->position.x, &packet[i+2], 4);
			memcpy(&temp->position.y, &packet[i+6], 4);
			memcpy(&temp->position.z, &packet[i+10], 4);

			memcpy(&temp->velocity.x, &packet[i+14], 4);
			memcpy(&temp->velocity.y, &packet[i+18], 4);
			memcpy(&temp->velocity.z, &packet[i+22], 4);

			memcpy(&temp->acceleration.x, &packet[i+26], 4);
			memcpy(&temp->acceleration.y, &packet[i+30], 4);
			memcpy(&temp->acceleration.z, &packet[i+34], 4);

			memcpy(&temp->horizontalRotation, &packet[i+38], 4);

			memcpy(&temp->animationIndex, &packet[i+42], 2);
			
			i+=GO_SIZE;
		}
	}
}

int Packet::getBufferSize() { return GO_SIZE * gameObjects->size(); }

GameObject * Packet::getGameObject(short ID)
{
	list<GameObject *>::iterator iter;

	for (iter = gameObjects->begin(); iter != gameObjects->end(); iter++)
		if ((*iter)->ID == ID) 
			{
				printf ("%i\n", ID);
				return *iter;
			}

	return NULL;
}

void Packet::setList(list<GameObject *> * goList) { gameObjects = goList; }