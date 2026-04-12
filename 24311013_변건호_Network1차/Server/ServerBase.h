#pragma once
#include "..\NetworkBase\NetworkBase.h"
#include "..\NetworkBase\NetSignal.h"

#include "IServerInitialize.h"


class ServerBase : public NetworkBase, public IServerInitialize
{
private:
	unsigned short m_port;

public:
	ServerBase(unsigned short port);
	virtual ~ServerBase() = default;

protected:
	NetInitResult TryBind() override;
	NetInitResult TryListen() override;

public:
	virtual NetInitResult NetInitialize() override;

	// 연결 성공한 클라이언트 소켓 반환, 실패 시 INVALID_SOCKET 반환
	SOCKET TryAccept() override;
};