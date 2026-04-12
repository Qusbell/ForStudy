#pragma once

#include "ClientBase.h"
#include <thread>

// 사용자 정의 메시지: 데이터 수신 알림
#define WM_RECV_DATA (WM_USER + 1)


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

public:
	ClientManager(const HWND hMainWnd);
	~ClientManager();

	NetInitResult TryStart(const std::string& ip, unsigned short port);

private:

	void RecvThread();

};

