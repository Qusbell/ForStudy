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
	m_isRunning = false;

	if (m_server != nullptr)
	{
		delete m_server;
		m_server = nullptr;
	}

	// NetSignal 객체들도 모두 삭제
	for (NetSignal* signal : m_signals)
	{
		if (signal != nullptr)
		{ delete signal; }
	}

	// 크리티컬 섹션 삭제
	DeleteCriticalSection(&m_signalsLock);
}


NetInitResult ServerManager::TryStart()
{
	auto result = GetServer().NetInitialize();

	if (result == NetInitResult::Complete)
	{
		m_isRunning = true;

		// 클라이언트 연결 수락 쓰레드 시작
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

			RegisterSignal(newSignal);

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
		else { break; }
	}

	// 연결 종료 또는 오류 발생 시 해당 신호 제거
	UnregisterSignal(signal);

	delete signal;
}


void ServerManager::Broadcast(const std::string& message)
{
	EnterCriticalSection(&m_signalsLock);

	// 모든 대상에 메시지 전송 시도
	for (NetSignal* signal : m_signals)
	{
		if (signal != nullptr)
		{
			signal->TrySend(message);
		}
	}

	LeaveCriticalSection(&m_signalsLock);
}


void ServerManager::RegisterSignal(NetSignal* newSignal)
{
	EnterCriticalSection(&m_signalsLock);
	m_signals.push_back(newSignal);
	LeaveCriticalSection(&m_signalsLock);
}

void ServerManager::UnregisterSignal(NetSignal* signal)
{
	EnterCriticalSection(&m_signalsLock);
	m_signals.erase(std::remove(m_signals.begin(), m_signals.end(), signal), m_signals.end());
	LeaveCriticalSection(&m_signalsLock);
}
