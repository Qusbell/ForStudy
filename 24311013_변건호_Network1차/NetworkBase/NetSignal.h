#pragma once
#include "INetSignal.h"
#include "NetRunner.h"

// 할당받은 소켓으로 1:1 통신만을 전담하는 클래스
class NetSignal : public INetSignal
{
protected:
	// 연결된 소켓
	SOCKET m_hSocket;

	// TCP 패킷 조립기
	NetRunner m_runner;

private:
	// 복사 금지
	NetSignal(const NetSignal&) = delete;
	NetSignal& operator=(const NetSignal&) = delete;

public:
	NetSignal(SOCKET hSocket);
	virtual ~NetSignal() override;

	virtual int TrySend(const PacketHeader& packet) override;
	virtual int TryRecv(std::function<void(char*)> onPacketReceived) override;
};