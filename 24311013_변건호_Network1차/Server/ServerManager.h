#pragma once
#include "ServerBase.h"
#include <vector>
#include <thread>

class ServerManager
{
private:
	// 서버 객체
    ServerBase* m_server;
	inline ServerBase& GetServer() const { return *m_server; }

	// 서버가 실행 중인지 여부
	bool m_isRunning;

	// 연결된 클라이언트 소켓과 통신을 담당하는 NetSignal 객체들의 리스트
	std::vector<NetSignal*> m_signals;

	// m_signals 리스트에 대한 동기화 객체
	CRITICAL_SECTION m_signalsLock;

private:
	// 복사 금지
	ServerManager(const ServerManager&) = delete;
	ServerManager& operator=(const ServerManager&) = delete;

public:
	ServerManager(unsigned short port);
	~ServerManager();

public:
	// 서버 시작 시도. 실패 시 실패 원인 반환
	NetInitResult TryStart();

	// 서버가 실행 중인지 여부 반환
	inline bool IsRunning() const { return m_isRunning; }

private:

	// 서버가 실행 중일 때, 클라이언트의 연결 요청을 수락하는 루프
	void AcceptThread();

	void RecvThread(NetSignal* signal);

	void Broadcast(const std::string& message);


private:
	void RegisterSignal(NetSignal* newSignal);
	void UnregisterSignal(NetSignal* signal);
};

