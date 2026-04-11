#pragma once
#include <string>
#include <ws2tcpip.h>

#include "..\NetworkBase\NetworkBase.h"


class ClientBase : public NetworkBase
{
private:
	std::string m_serverIp; // 접속할 서버의 IP 주소
	unsigned short m_serverPort; // 접속할 서버의 포트 번호

public:
	// 생성자에서 서버의 IP와 포트를 받습니다.
	ClientBase(const std::string& ip, unsigned short port);
	virtual ~ClientBase() = default;

private:
	// 서버로 연결을 시도하는 내부 함수
	NetInitResult TryConnect();

public:
	virtual NetInitResult NetInitialize() override;
};