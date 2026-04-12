#include "ClientManager.h"

ClientManager::ClientManager(const HWND hMainWnd) :
	m_client(nullptr),
	m_hMainWnd(hMainWnd),
	m_signal(nullptr),
	m_isRunning(false)
{}


ClientManager::~ClientManager()
{
	m_isRunning = false;

	if (m_client != nullptr)
	{
		delete m_client;
		m_client = nullptr;
	}

	if(m_signal != nullptr)
	{
		delete m_signal;
		m_signal = nullptr;
	}
}

NetInitResult ClientManager::TryStart(const std::string& ip, unsigned short port)
{
	// 1. 이미 클라이언트가 실행 중인 경우, 바로 Complete 반환
	if (m_isRunning) { return NetInitResult::Complete; }

	// 2. 클라이언트 객체 생성 및 초기화
	m_client = new ClientBase(ip, port);
	auto result = GetClient().NetInitialize();

	// 3. 초기화 결과에 따라 처리
	if (result == NetInitResult::Complete)
	{
		m_isRunning = true;
		SOCKET socket = GetClient().GetSocket();
		m_signal = new NetSignal(socket);

		// <-- recv 쓰레드 시작

		// <-- send 쓰레드 시작
	}
	// 초기화 실패 시, 클라이언트 객체 정리
	else
	{
		delete m_client;
		m_client = nullptr;
	}

	// 4. 초기화 결과 반환
	return result;
}


void ClientManager::RecvThread()
{
	while (m_isRunning)
	{
		std::string recvBuffer;
		if (0 < m_signal->TryRecv(recvBuffer)) {
			//std::string* pMsg = new std::string(recvBuffer);
			PostMessage(m_hMainWnd, WM_RECV_DATA, 0, (LPARAM)&recvBuffer);
		}
		else { break; }
	}
}
