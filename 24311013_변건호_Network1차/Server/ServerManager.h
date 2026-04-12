#pragma once
#include "ServerBase.h"
#include <vector>

class ServerManager
{
private:
	// 서버 객체
    ServerBase* m_server;
	inline ServerBase& GetServer() { return *m_server; }

	// 연결된 클라이언트 소켓과 통신을 담당하는 NetSignal 객체들의 리스트
	std::vector<NetSignal*> m_signals;

public:
	ServerManager(unsigned short port);
	~ServerManager();

public:
	// 서버 시작 시도. 실패 시 실패 원인 반환
	NetInitResult TryStart();



};

