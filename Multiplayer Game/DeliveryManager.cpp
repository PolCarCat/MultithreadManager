#include "Networks.h"
#include "DeliveryManager.h"

Delivery * DeliveryManager::WriteSequenceNumber(OutputMemoryStream & packet)
{

	packet << nextSeqNumber;

	return nullptr;
}

bool DeliveryManager::ProccesSequenceNumber(const InputMemoryStream & packet)
{
	packet >> nextExpNumber;

	return false;
}

bool DeliveryManager::HasSequenceNumbersPendingAck() const
{
	return !pendingAckNumbers.empty();
}

void DeliveryManager::WriteSequenceNumbersPendingAck(OutputMemoryStream & packet)
{

}

void DeliveryManager::ProcessAckdSequenceNumbers(const InputMemoryStream & packet)
{

}

void DeliveryManager::ProcessTimeOutPackets()
{

}

void DeliveryManager::Clear()
{

}
