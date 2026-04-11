#pragma once
#include "..\NetworkBase\NetworkBase.h"

class ServerBase : public NetworkBase
{
private:
	unsigned short m_port;

public:
	ServerBase(unsigned short port);
	virtual ~ServerBase() = default;

private:
	NetInitResult TryBind();
	NetInitResult TryListen();

public:
	virtual NetInitResult NetInitialize() override;

	SOCKET TryAccept();
};