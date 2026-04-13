#pragma once

#include "ClientBase.h"
#include <thread>
#include <queue>
#include <string>

// 사용자 정의 메시지: 데이터 수신 알림
#define WM_RECV_DATA (WM_USER + 1)
// 사용자 정의 메시지: ID 수신 알림
#define WM_RECV_ID (WM_USER + 2)

class ClientManager
{
private:
	// 클라이언트 객체
	ClientBase* m_client;
	inline ClientBase& GetClient() { return *m_client; }

	// 통신 신호 객체
	NetSignal* m_signal;

	// UI 핸들 (메인 윈도우)
	const HWND m_hMainWnd;

	// 클라이언트가 실행 중인지 여부
	bool m_isRunning;

	// 수신된 메시지를 저장하는 큐
	std::queue<std::string> m_recvQueue;
	// 큐 접근 동기화용 크리티컬 섹션
	CRITICAL_SECTION m_recvQueueLock;

public:
	ClientManager(const HWND hMainWnd);
	~ClientManager();

	// 접속 시도 시 UI에서 입력한 이름(name)을 함께 받도록 수정
	NetInitResult TryStart(const std::string& ip, unsigned short port, const std::string& name);

private:

	// 서버로부터 데이터를 수신하는 쓰레드 함수 (단일)
	void RecvThread();


public:
	// 서버로 데이터를 전송하는 함수 (비-쓰레드)
	void TrySendChat(const std::string& message);

	const std::string GetRecvMessage();
};