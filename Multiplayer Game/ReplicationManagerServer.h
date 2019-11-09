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

private:
	std::vector<ReplicationCommand> commands;

};

