#pragma once

#define PACKET_MAGIC 0x50434B54 // 'PCKT'


enum class PacketType : int
{
	//=== 초기화 ===//
	On_CONNECT = 1, // 클라 --> 서버 : name 알려줌
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
	int magic = PACKET_MAGIC;
	// 패킷 총 사이즈
	int size;
	// 패킷의 종류 (맥락)
	PacketType type;
};
#pragma pack(pop)



#pragma pack(push, 1)
struct PacketOnConnect
{
	PacketHeader header;
	char name[20];
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
	char chat[512];
	// std::string chat; // <-- 이건 가변형이라 안 되나?
};
#pragma pack(pop)



#pragma pack(push, 1)
struct PacketBoradcastChat
{
	PacketHeader header;
	char name[20];
	char chat[512];
};
#pragma pack(pop)
