#include "ServerManager.h"

ServerManager::ServerManager(unsigned short port)
{
	m_server = new ServerBase(port);
	m_isRunning = false;

	// 크리티컬 섹션 초기화
	InitializeCriticalSection(&m_signalsLock);
}

ServerManager::~ServerManager()
{
	delete m_server;

	// NetSignal 객체들도 모두 삭제
	for (NetSignal* signal : m_signals)
	{
		if (signal != nullptr)
		{ delete signal; }
	}

	m_isRunning = false;

	DeleteCriticalSection(&m_signalsLock);
}


NetInitResult ServerManager::TryStart()
{
	auto result = GetServer().NetInitialize();

	if (result == NetInitResult::Complete)
	{
		m_isRunning = true;

		// 쓰레드 시작
		std::thread acceptThread(&ServerManager::AcceptThread, this);
		acceptThread.detach();
	}

	return result;
}


void ServerManager::AcceptThread()
{
	while (m_isRunning)
	{
		SOCKET hClient = GetServer().TryAccept();

		if(hClient != INVALID_SOCKET)
		{
			// 새로운 클라이언트 연결 처리
			NetSignal* newSignal = new NetSignal(hClient);

			EnterCriticalSection(&m_signalsLock);
			m_signals.push_back(newSignal);
			LeaveCriticalSection(&m_signalsLock);

			// 대상 Sinal로부터 recv하는 쓰레드 생성
			std::thread recvThread(&ServerManager::RecvThread, this, newSignal);
			recvThread.detach();
		}
	}
}


void ServerManager::RecvThread(NetSignal* signal)
{
	NetSignal& threadSignal = *signal;

	while (m_isRunning)
	{
		std::string recvBuffer;
		if (0 < threadSignal.TryRecv(recvBuffer))
		{
			// 받은 메시지를 모든 클라이언트에게 브로드캐스트
			Broadcast(recvBuffer);
		}
		else
		{
			// 연결 종료 또는 오류 발생 시 해당 신호 제거
			break;
		}
	}

	delete signal; // NetSignal 객체 삭제
}


void ServerManager::Broadcast(const std::string& message)
{
	EnterCriticalSection(&m_signalsLock);

	// 1. 유효하지 않은(nullptr) 신호들을 먼저 리스트에서 완전히 제거 (반복문 밖에서 수행)
	m_signals.erase(std::remove(m_signals.begin(), m_signals.end(), nullptr), m_signals.end());

	// 2. 모든 유효한 신호에 메시지 전송 시도
	for (NetSignal* signal : m_signals)
	{
		if (signal != nullptr)
		{
			signal->TrySend(message);
		}
	}

	LeaveCriticalSection(&m_signalsLock);
}
