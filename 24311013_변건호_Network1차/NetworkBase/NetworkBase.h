#pragma once
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

#include <winsock2.h>
#include <windows.h>

#include "INetInitialize.h"
#include "INetSignal.h"


class NetworkBase : public INetInitialize, public INetSignal
{
private:
	// win 소켓
	WSADATA m_wsaData;

	bool m_wsaInitFlag;

	// 연결 소켓
	SOCKET m_hSocket;

protected:
	SOCKET GetSocket() const;

	void TryWSACleanUp();
	void TryCloseSocket();

public:
	NetworkBase();
	virtual ~NetworkBase();

private:
	NetInitResult InitializeWSA();
	NetInitResult InitializeSocket();

public:
	virtual NetInitResult NetInitialize() override;

	virtual int Send(const char* buffer, int len) override;
	virtual int Recv(char* buffer, int len) override;
};

