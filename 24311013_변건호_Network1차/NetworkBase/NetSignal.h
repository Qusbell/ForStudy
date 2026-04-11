#pragma once
#include "NetworkBase.h"


// 할당받은 소켓으로 1:1 통신만을 전담하는 클래스
class NetSignal : public INetSignal
{
protected:
	// 연결된 소켓
	SOCKET m_hSocket;

public:
	NetSignal(SOCKET hSocket);
	virtual ~NetSignal() = default;

	virtual int TrySend(const char* buffer) override;
	virtual int TryRecv(char* buffer) override;
};
