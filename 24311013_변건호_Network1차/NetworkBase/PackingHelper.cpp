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



//=== 내부 안전 추출 헬퍼 구현 ===//
std::string PackingHelper::SafeGetString(const char* arr, const int maxSize)
{
	// 널 문자가 없어도 maxSize까지만 읽도록 강제하여 메모리 오버플로우 방지
	size_t len = strnlen(arr, maxSize);
	return std::string(arr, len);
}

bool PackingHelper::IsValidPacket(const PacketHeader& header, const PacketType type)
{
	bool isMagicValid = header.magic == PACKET_MAGIC;
	bool isTypeValid = header.type == type;
	bool isSizeValid = false;

	switch (type)
	{
	case PacketType::ON_CONNECT:
		isSizeValid == sizeof(PacketOnConnect);
		break;
		
	case PacketType::ASSIGN_ID:
		isSizeValid == sizeof(PacketAssignID);
		break;

	case PacketType::REQUEST_CHAT:
		isSizeValid = sizeof(PacketRequestChat);
		break;

	case PacketType::BROADCAST_CHAT:
		isSizeValid = sizeof(PacketBroadcastChat);
		break;

	default:
		isSizeValid = false;
		break;
	}

	return isMagicValid && isTypeValid && isSizeValid;
}

//=== Unpack 헬퍼 구현 ===//

bool PackingHelper::Unpack_OnConnect(const void* buffer, std::string& out_name)
{
	if (buffer == nullptr) { return false; }
	const PacketOnConnect* packet = static_cast<const PacketOnConnect*>(buffer);
	if (!IsValidPacket(packet->header, PacketType::ON_CONNECT)) { return false; }

	out_name = SafeGetString(packet->name, NAME_MAX_SIZE);

	return true;
}

bool PackingHelper::Unpack_AssignID(const void* buffer, int& out_ID)
{
	if (buffer == nullptr) { return false; }
	const PacketAssignID* packet = static_cast<const PacketAssignID*>(buffer);
	if (!IsValidPacket(packet->header, PacketType::ASSIGN_ID)) { return false; }

	out_ID = packet->ID;

	return true;
}

bool PackingHelper::Unpack_RequestChat(const void* buffer, std::string& out_chat)
{
	if (buffer == nullptr) { return false; }

	const PacketRequestChat* packet = static_cast<const PacketRequestChat*>(buffer);
	if (!IsValidPacket(packet->header, PacketType::REQUEST_CHAT)) { return false; }

	out_chat = SafeGetString(packet->chat, CHAT_MAX_SIZE);

	return true;
}

bool PackingHelper::Unpack_BroadcastChat(const void* buffer, std::string& out_name, std::string& out_chat)
{
	if (buffer == nullptr) { return false; }

	const PacketBroadcastChat* packet = static_cast<const PacketBroadcastChat*>(buffer);
	if (!IsValidPacket(packet->header, PacketType::BROADCAST_CHAT)) { return false; }

	out_name = SafeGetString(packet->name, NAME_MAX_SIZE);
	out_chat = SafeGetString(packet->chat, CHAT_MAX_SIZE);

	return true;
}