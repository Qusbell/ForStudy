#include "ClientManager.h"

ClientManager::ClientManager(const std::string& ip, unsigned short port)
{
	// 1. 클라이언트 객체 생성
	m_client = new ClientBase(ip, port);

	// 2. 클라이언트 초기화 시도
	NetInitResult result = m_client->NetInitialize();
	if (result != NetInitResult::Complete)
	{
		// 초기화 실패 시 예외 처리 (여기서는 간단히 콘솔 출력)
		delete m_client; // 할당된 메모리 해제
		m_client = nullptr;
		m_isRunning = false;
		return;
	}
	m_isRunning = true; // 초기화 성공 시 실행 상태로 설정
}


ClientManager::~ClientManager()
{
	m_isRunning = false;

	if (m_client != nullptr)
	{
		delete m_client;
		m_client = nullptr;
	}
}

NetInitResult ClientManager::TryStart()
{
	NetInitResult result = GetClient().NetInitialize();

	if (result == NetInitResult::Complete)
	{
		m_isRunning = true;
		SOCKET socket = GetClient().GetSocket();

		// <-- recv 쓰레드 시작

		// <-- send 쓰레드 시작
	}

	return result;
}
