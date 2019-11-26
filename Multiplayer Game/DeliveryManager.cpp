#include "Networks.h"
#include "DeliveryManager.h"

Delivery * DeliveryManager::WriteSequenceNumber(OutputMemoryStream & packet)
{
	Delivery* newDel = new Delivery();

	packet << nextSeqNumber;

	newDel->sequenceNumber = nextSeqNumber;
	newDel->dispatchTime = Time.time;

	pendingDeliveries.push_back(newDel);

	return nullptr;
}

bool DeliveryManager::ProccesSequenceNumber(const InputMemoryStream & packet)
{
	uint32 seqNumber = 0;
	packet >> seqNumber;

	if (seqNumber == nextExpNumber)
	{
		pendingAckNumbers.push_back(seqNumber);

		nextExpNumber++;
		return true;
	}

	return false;
}

bool DeliveryManager::HasSequenceNumbersPendingAck() const
{
	return !pendingAckNumbers.empty();
}

void DeliveryManager::WriteSequenceNumbersPendingAck(OutputMemoryStream & packet)
{
	for (int i = 0; i < pendingAckNumbers.size(); i++)
	{
		packet << pendingAckNumbers[i];
	}

	pendingAckNumbers.clear();
}

void DeliveryManager::ProcessAckdSequenceNumbers(const InputMemoryStream & packet)
{

	while (packet.RemainingByteCount() > 0) 
	{
		uint32 seqNumber = -1;

		packet >> seqNumber;

		for (int i = 0; i < pendingDeliveries.size();i++)
		{
			if (pendingDeliveries[i]->sequenceNumber == seqNumber) 
			{
				pendingDeliveries[i]->delegate->OnDeliverySuccess(this);
				pendingDeliveries.erase(pendingDeliveries.begin() + i);

				break;

			}
		}
	}
}

void DeliveryManager::ProcessTimeOutPackets()
{
	std::vector<int> toDelete;

	for (int i = 0; i < pendingDeliveries.size(); i++)
	{
		if (Time.time > pendingDeliveries[i]->dispatchTime + PACKET_DELIVERY_TIMEOUT_SECONDS)
		{
			toDelete.push_back(i);
		}
	}

	for (int i = 0; i < toDelete.size(); i++)
	{
		int index = toDelete.size() - i;

		pendingDeliveries[index]->delegate->OnDeliveryFailure(this);
		pendingDeliveries.erase(pendingDeliveries.begin() + index);
	}
}

void DeliveryManager::Clear()
{

	while (!pendingDeliveries.empty()) delete pendingDeliveries.back(), pendingDeliveries.pop_back();

	pendingAckNumbers.clear();

	nextSeqNumber = 0;
	nextExpNumber = 0;

}
