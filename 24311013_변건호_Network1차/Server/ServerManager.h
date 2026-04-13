#pragma once
#include "ServerBase.h"
#include <vector>
#include <thread>
#include <string>

// 과제 요구사항인 클라이언트 정보 관리용 구조체
struct ClientSession
{
	int id;
	std::string name;
	NetSignal* signal;
};

class ServerManager
{
private:
	// 서버 객체
	ServerBase* m_server;
	inline ServerBase& GetServer() const { return *m_server; }

	// 서버가 실행 중인지 여부
	bool m_isRunning;

	// 연결된 클라이언트 세션 관리 리스트
	std::vector<ClientSession*> m_sessions;
	// m_sessions 리스트에 대한 동기화 객체
	CRITICAL_SECTION m_sessionsLock;

private:
	// 복사 금지
	ServerManager(const ServerManager&) = delete;
	ServerManager& operator=(const ServerManager&) = delete;

public:
	ServerManager();
	~ServerManager();

public:
	// 서버 시작 시도. 실패 시 실패 원인 반환
	NetInitResult TryStart(unsigned short port);

	// 서버가 실행 중인지 여부 반환
	inline bool IsRunning() const { return m_isRunning; }

private:
	// 서버가 실행 중일 때, 클라이언트의 연결 요청을 수락하는 루프 (단일)
	void AcceptThread();

	// 각 담당 클라이언트 소켓마다 할당되는 쓰레드 (다수)
	void RecvThread(ClientSession* session);

	// 패킷을 모든 클라이언트에게 전송
	void Broadcast(const PacketHeader& packet);

private:
	void RegisterSession(ClientSession* newSession);
	void UnregisterSession(ClientSession* session);
};