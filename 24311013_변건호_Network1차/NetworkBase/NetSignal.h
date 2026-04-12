#pragma once
#include "INetSignal.h"
#include "PacketHeader.h"


// 할당받은 소켓으로 1:1 통신만을 전담하는 클래스
class NetSignal : public INetSignal
{
protected:
	// 연결된 소켓
	SOCKET m_hSocket;

private:
	// 복사 금지
	NetSignal(const NetSignal&) = delete;
	NetSignal& operator=(const NetSignal&) = delete;

public:
	NetSignal(SOCKET hSocket);
	virtual ~NetSignal() override;

	virtual int TrySend(const std::string& buffer) override;

	virtual int TryRecv(std::string& buffer) override;
};
