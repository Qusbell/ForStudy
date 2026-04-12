#pragma once

#define PACKET_MAGIC 0x50434B54 // 'PCKT'


enum class PacketType : int
{
	CONNECT = 1,
	CONNECT_ACK,


	//=== C to S (Request) ===//

	CHAT = 0x1001,


	//=== S to C (Broadcast) ===//

	CHAT_BROADCAST = 0x2001
};


#pragma pack(push, 1)
struct PacketHeader
{
	// 패킷 키
	int magic = PACKET_MAGIC;
	// 패킷 총 사이즈
	int size;
	// 패킷의 종류 (맥락)
	PacketType type;
};
#pragma pack(pop)


