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

			//packet >> newGO->tag;
			packet >> newGO->position.x;
			packet >> newGO->position.y;
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
