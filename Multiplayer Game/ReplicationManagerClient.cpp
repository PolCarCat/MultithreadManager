#include "Networks.h"
#include "ReplicationManagerClient.h"

void ReplicationManagerClient::read(const InputMemoryStream & packet)
{

	while (packet.GetSize() > 0) 
	{
		ReplicationAction action;
		uint32 id;

		packet >> action;
		packet >> id;


		switch (action)
		{
		case ReplicationAction::Create:
		{
			GameObject* newGO = Instantiate();
			App->modLinkingContext->registerNetworkGameObject(newGO);

			//packet >> newGO->position;
			packet >> newGO->tag;
		}
			break;
		case ReplicationAction::Update:
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(id);

			//packet >> go->position;
			packet >> go->tag;
		}	
			break;
		case ReplicationAction::Destroy:
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(id);
			App->modLinkingContext->unregisterNetworkGameObject(go);
			Destroy(go);

		}
			break;
		}
	}
}
