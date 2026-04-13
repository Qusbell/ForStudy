#pragma once

#include <winsock2.h>
#include <functional>

#include "PacketHeader.h"
#include "PackingHelper.h"

#define RECV_BUFFER_SIZE 8192

class NetRunner
{
private:
	// TCP 스트림 수신 상태를 유지하기 위한 내부 버퍼 변수
	char m_recvBuffer[RECV_BUFFER_SIZE];
	int m_remainSize;

public:
	NetRunner();
	~NetRunner() = default;

	// @param target: 전송할 소켓
	// @param packet: 전송할 패킷 포인터 (PacketHeader 메모리 연속성 활용)
	int TrySend(const SOCKET target, const PacketHeader& packet);

	// @param target: 수신할 소켓
	// @param onPacketReceived: 완전한 패킷 1개가 조립되었을 때 호출될 콜백 함수
	// @return : 수신된 바이트 수. 0은 연결 종료, 음수는 오류
	int TryRecv(const SOCKET target, std::function<void(char*)> onPacketReceived);
};