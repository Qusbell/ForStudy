#pragma once
#include <winsock2.h>
#include <functional>
#include "PacketHeader.h"

class INetSignal
{
public:
	virtual ~INetSignal() = default;

	// @param packet: 전송할 패킷의 헤더 주소 (PackingHelper로 패키징된 구조체)
	// @return : 실제로 전송된 바이트 수. 음수는 오류
	virtual int TrySend(const PacketHeader& packet) = 0;

	// @param onPacketReceived: 완전한 패킷 1개가 조립되었을 때 호출될 콜백 함수
	// @return : 수신된 바이트 수. 0은 연결 종료, 음수는 오류
	virtual int TryRecv(std::function<void(char*)> onPacketReceived) = 0;
};