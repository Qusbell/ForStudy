#pragma once

#include <cstdint>

#define PACKET_MAGIC 0x50434B54 // 'PCKT'

#define NAME_MAX_SIZE 20
#define CHAT_MAX_SIZE 256


enum class PacketType : uint32_t
{
	// 오류
	NONE = 0,

	//=== 초기화 ===//
	ON_CONNECT = 1, // 클라 --> 서버 : name 알려줌
	ASSIGN_ID,      // 서버 --> 클라 : ID 부여

	//=== 클라 --> 서버 ===//
	REQUEST_CHAT = 0x1001,  // 채팅 요청

	//=== 서버 --> 클라 ===//
	BROADCAST_CHAT = 0x2001 // 전파
};


#pragma pack(push, 1)
struct PacketHeader
{
	// 패킷 키
	uint32_t magic = PACKET_MAGIC;
	// 패킷 총 사이즈
	uint16_t size = 0;
	// 패킷의 종류 (맥락)
	PacketType type = PacketType::NONE;
};
#pragma pack(pop)



#pragma pack(push, 1)
struct PacketOnConnect
{
	PacketHeader header;
	char name[NAME_MAX_SIZE];
};
#pragma pack(pop)



#pragma pack(push, 1)
struct PacketAssignID
{
	PacketHeader header;
	int ID;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct PacketRequestChat
{
	PacketHeader header;
	char chat[CHAT_MAX_SIZE];
};
#pragma pack(pop)


#pragma pack(push, 1)
struct PacketBroadcastChat
{
	PacketHeader header;
	char name[NAME_MAX_SIZE];
	char chat[CHAT_MAX_SIZE];
};
#pragma pack(pop)
