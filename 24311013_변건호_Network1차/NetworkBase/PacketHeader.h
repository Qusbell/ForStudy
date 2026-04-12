#pragma once

#define PACKET_MAGIC 0x50434B54 // 'PCKT'


enum class PacketType : int
{
	PKT_CONNECT = 1,
	PKT_CONNECT_ACK,
	PKT_CHAT = 0x1001,
	PKT_CHAT_BROADCAST = 0x2001,
};


#pragma pack(push, 1)
struct PacketHeader
{
	int magic = PACKET_MAGIC;
	int size;
	PacketType type;
};
#pragma pack(pop)
