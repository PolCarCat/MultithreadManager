#include "Networks.h"
#include "ReplicationManagerServer.h"

void ReplicationManagerServer::Create(uint32 networkId)
{
	AddAction(ReplicationAction::Create, networkId);

}

void ReplicationManagerServer::Update(uint32 networkId)
{

	if (!CheckId(networkId))
		AddAction(ReplicationAction::Update, networkId);

}

void ReplicationManagerServer::Destroy(uint32 networkId)
{

	AddAction(ReplicationAction::Destroy, networkId);

}

void ReplicationManagerServer::write(OutputMemoryStream & packet)
{
	//packet << ServerMessage::Replication;

	for (auto command : commands) 
	{
		packet << command.action;
		packet << command.networkId;

		switch (command.action) 
		{
		case (ReplicationAction::Create):
		{
			GameObject* newGameObject = App->modLinkingContext->getNetworkGameObject(command.networkId);
			//Serialize Fields
			if (newGameObject != nullptr) {
				//packet << newGameObject->tag;
				packet << newGameObject->position.x;
				packet << newGameObject->position.y;
				packet << newGameObject->size.x;
				packet << newGameObject->size.y;
				packet << newGameObject->angle;

				std::string texname = newGameObject->texture->filename;
				packet << texname;
			}

			//packet << std::string(newGameObject->texture->filename);
		}
		break;
		case (ReplicationAction::Update):
		{
			GameObject* newGameObject = App->modLinkingContext->getNetworkGameObject(command.networkId);
			//Serialize Fields

			float posX = 0;
			float posY = 0;
			float angle = 0;
			if (newGameObject != nullptr) 
			{
				//packet << newGameObject->tag;
				packet << newGameObject->position.x;
				packet << newGameObject->position.y;
				packet << newGameObject->angle;		
			}
			else 
			{
				packet << posX;
				packet << posY;
				packet << angle;
			}

		}
		break;
		}
	}

	commands.clear();
}



void ReplicationManagerServer::AddAction(ReplicationAction action, uint32 networkId)
{

	for (int i = 0; i < commands.size(); i++) {
		if (commands[i].networkId == networkId) {
			commands[i].action = action;
			return;
		}
	}

	ReplicationCommand newCommand;

	newCommand.action = action;
	newCommand.networkId = networkId;

	commands.push_back(newCommand);
}

bool ReplicationManagerServer::CheckId(uint32 networkId)
{

	for (auto command : commands) 
	{
		if (command.networkId == networkId) return true;
	}
	return false;
}
