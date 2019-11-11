#include "Networks.h"
#include "ReplicationManagerClient.h"

void ReplicationManagerClient::read(const InputMemoryStream & packet)
{

	while (packet.RemainingByteCount() > 0) 
	{
		ReplicationAction action;
		uint32 id;

		packet >> action;
		packet >> id;

		int bytes = packet.RemainingByteCount();

		switch (action)
		{
		case ReplicationAction::Create:
		{
			GameObject* newGO = Instantiate();
			App->modLinkingContext->registerNetworkGameObject(newGO);

			//packet >> newGO->position;
			packet >> newGO->tag;
			packet >> newGO->position.x;
			packet >> newGO->position.y;
			//packet >> newGO->angle;

			//std::string textname;
			//packet >> textname;
			//newGO->texture->filename = textname.c_str();
		}
			break;
		case ReplicationAction::Update:
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(id);

			//packet >> go->position;
			if (go != nullptr) 
			{
				packet >> go->tag;
				packet >> go->position.x;
				packet >> go->position.y;
				//packet >> go->angle;

				//std::string textname;
				//packet >> textname;
				//go->texture->filename = textname.c_str();

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
