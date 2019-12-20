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

			uint8 team;
			packet >> team;

			Behaviour::BehaviourType type;
			packet >> type;

			if (id == clientNetID)
			{
				Spaceship* behavior = new Spaceship((Behaviour::Team)team);
				behavior->gameObject = newGO;
				behavior->isServer = false;
				newGO->behaviour = behavior;
			}
			else {
				newGO->behaviour = new Behaviour((Behaviour::Team)team);
				newGO->behaviour->isServer = false;
			}



			if (type == Behaviour::BehaviourType::BULLET) {
				newGO->behaviour = new Laser((Behaviour::Team)team);
				newGO->behaviour->gameObject = newGO;
				newGO->behaviour->isServer = false;
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

				vec2 pos = { 0.0f, 0.0f };
				float angle = 0.0f;

				packet >> pos.x;
				packet >> pos.y;
				packet >> angle;

				//The health of the others players it's represented in the size 
				packet >> go->size.x;
				packet >> go->size.y;

				if (id != clientNetID)
				{
					go->ResetPos(pos, angle);
				}
				else {
					go->position = pos;
					go->angle = angle;
				}


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
