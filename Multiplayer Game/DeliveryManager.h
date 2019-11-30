#pragma once


class DeliveryManager;
class ReplicationCommand;
class ReplicationManagerServer;

class DeliveryDelegate
{
public:
	virtual void OnDeliverySuccess(DeliveryManager* deliveryManager) = 0;
	virtual void OnDeliveryFailure(DeliveryManager* deliveryManager) = 0;
	void AddCommands(std::vector<ReplicationCommand> c);

	std::vector<ReplicationCommand> commands;

	ReplicationManagerServer* repServer;
};


class DeliveryDelegateReplication : public DeliveryDelegate
{
public:
	void OnDeliverySuccess(DeliveryManager* deliveryManager) override;
	void OnDeliveryFailure(DeliveryManager* deliveryManager) override;


};

class Delivery
{
public:
	uint32 sequenceNumber = 0;
	double dispatchTime = 0;
	DeliveryDelegate* delegate = nullptr;
};

class DeliveryManager 
{
public:

	//For senders to write a new seq. numbers into a packet
	Delivery* WriteSequenceNumber(OutputMemoryStream& packet);

	//For recievers to process the seq. number from an incoming packet
	bool ProccesSequenceNumber(const InputMemoryStream& packet);

	//For recievers to write ack'ed seq. numbers into a packet
	bool HasSequenceNumbersPendingAck() const;
	void WriteSequenceNumbersPendingAck(OutputMemoryStream& packet);

	//For sensers to process ack'ed seq. numbers from packet
	void ProcessAckdSequenceNumbers(const InputMemoryStream& packet);
	void ProcessTimeOutPackets();

	void Clear();

private:

	//Private members sender side 
	// -The next outgoing sequence number
	// -A list of pending deliveries

	uint32 nextSeqNumber = 0;
	std::vector<Delivery*> pendingDeliveries;

	//Private members reciever side
	// -Next expeced sequence number
	// -A list of sequence numbers pending to ack
	uint32 nextExpNumber = 0;
	std::vector<uint32> pendingAckNumbers;


};
