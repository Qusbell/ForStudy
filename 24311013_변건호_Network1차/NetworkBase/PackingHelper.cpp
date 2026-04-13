#include "PackingHelper.h"

void PackingHelper::Packing_Header(PacketHeader& header, const PacketType type, const uint16_t sizeofPacket)
{
	// 초기화
	header = { 0 };

	header.magic = PACKET_MAGIC;
	header.type = type;
	header.size = sizeofPacket;
}

void PackingHelper::Packing_CharArr(char* packetArr, const int arrSize, const std::string& packingTarget)
{
	// 초기화
	memset(packetArr, 0, arrSize);

	std::size_t copyLength = std::min(packingTarget.size(), (std::size_t)(arrSize - 1));
	packingTarget.copy(packetArr, copyLength);
}

bool PackingHelper::Packing_OnConnect(PacketOnConnect& packet, const std::string& name)
{
	if (name.empty()) { return false; }

	Packing_Header(packet.header, PacketType::ON_CONNECT, sizeof(PacketOnConnect)); // 헤더 패키징
	Packing_CharArr(packet.name, NAME_MAX_SIZE, name); // 데이터 패키징

	return true;
}

void PackingHelper::Packing_AssignID(PacketAssignID& packet, const int ID)
{
	Packing_Header(packet.header, PacketType::ASSIGN_ID, sizeof(PacketOnConnect)); // 헤더 패키징
	packet.ID = ID;
}

bool PackingHelper::Packeting_RequestChat(PacketRequestChat& packet, const std::string& chat)
{
	if (chat.empty()) { return false; }

	Packing_Header(packet.header, PacketType::REQUEST_CHAT, sizeof(PacketRequestChat)); // 헤더 패키징
	Packing_CharArr(packet.chat, CHAT_MAX_SIZE, chat); // 데이터 패키징

	return true;
}

bool PackingHelper::Packing_BroadcastChat(PacketBroadcastChat& packet, const std::string& name, const std::string& chat)
{
	if (chat.empty()) { return false; }

	Packing_Header(packet.header, PacketType::BROADCAST_CHAT, sizeof(PacketBroadcastChat)); // 헤더 패키징
	Packing_CharArr(packet.name, NAME_MAX_SIZE, name); // 데이터 패키징
	Packing_CharArr(packet.chat, CHAT_MAX_SIZE, chat); // 데이터 패키징

	return true;
}