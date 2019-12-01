#include "Networks.h"
#include "ReplicationManagerClient.h"

void ReplicationManagerClient::read(const InputMemoryStream & packet, uint32 clientNetID)
{

	while (packet.RemainingByteCount() > 0)
	{
		ReplicationAction action;
		uint32 id;

		packet >> action;
		packet >> id;


		switch (action)
		{
		case ReplicationAction::Create:
		{
			GameObject* newGO = App->modLinkingContext->getNetworkGameObject(id);

			if (!newGO)
			{
				newGO = Instantiate();
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(newGO, id);
			}

			if (id == clientNetID)
			{
				Spaceship* behavior = new Spaceship();
				behavior->gameObject = newGO;
				behavior->isServer = false;
				newGO->behaviour = behavior;
			}

			//packet >> newGO->tag;
			packet >> newGO->position.x;
			packet >> newGO->position.y;
			packet >> newGO->size.x;
			packet >> newGO->size.y;
			packet >> newGO->angle;

			std::string texname;
			packet >> texname;
			newGO->texture = App->modTextures->loadTexture(texname.c_str());
				

		}
			break;
		case ReplicationAction::Update:
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(id);

			if (go != nullptr) 
			{
				//packet >> go->tag;
				packet >> go->position.x;
				packet >> go->position.y;
				packet >> go->angle;

				//The health of the others players it's represented in the size 
				packet >> go->size.x;
				packet >> go->size.y;

			}


		}	
			break;
		case ReplicationAction::Destroy:
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(id);
			if (go != nullptr) 
			{
				App->modLinkingContext->unregisterNetworkGameObject(go);
				Destroy(go);

			}
		}
			break;
		}
	}
}
