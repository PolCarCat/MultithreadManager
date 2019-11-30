#pragma once

enum class ReplicationAction
{
	None,
	Create,
	Update,
	Destroy
};

struct ReplicationCommand 
{
	ReplicationAction action;
	uint32 networkId;
};

class ReplicationManagerServer
{
public:
	void Create(uint32 networkId);
	void Update(uint32 networkId);
	void Destroy(uint32 networkId);

	void write(OutputMemoryStream &packet);

	int CommandsSize() { return commands.size();  };

	std::vector<ReplicationCommand> GetCommands();

	void AddAction(ReplicationAction action, uint32 networkId);

private:
	std::vector<ReplicationCommand> commands;


	bool CheckId(uint32 networkId);

};

