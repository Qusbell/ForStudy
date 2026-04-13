#include "ServerManager.h"
#include <algorithm>

ServerManager::ServerManager() :
	m_server(nullptr),
	m_isRunning(false)
{
	// 크리티컬 섹션 초기화
	InitializeCriticalSection(&m_sessionsLock);
}

ServerManager::~ServerManager()
{
	m_isRunning = false;

	if (m_server != nullptr)
	{
		delete m_server;
		m_server = nullptr;
	}

	// 모든 세션 메모리 해제
	for (ClientSession* session : m_sessions)
	{
		if (session != nullptr)
		{
			if (session->signal != nullptr) delete session->signal;
			delete session;
		}
	}

	// 크리티컬 섹션 삭제
	DeleteCriticalSection(&m_sessionsLock);
}

NetInitResult ServerManager::TryStart(unsigned short port)
{
	// 1. 이미 서버가 실행 중인 경우, 바로 Complete 반환
	if (m_isRunning) { return NetInitResult::Complete; }

	// 2. 서버 객체 생성 및 초기화 시도
	m_server = new ServerBase(port);
	auto result = GetServer().NetInitialize();

	// 3. 초기화 결과에 따라 서버 실행 여부 결정
	if (result == NetInitResult::Complete)
	{
		m_isRunning = true;

		// 클라이언트 연결 수락 쓰레드 시작
		std::thread acceptThread(&ServerManager::AcceptThread, this);
		acceptThread.detach();
	}
	// 초기화 실패 시, 생성한 서버 객체 삭제
	else
	{
		delete m_server;
		m_server = nullptr;
	}

	// 4. 초기화 결과 반환
	return result;
}

void ServerManager::AcceptThread()
{
	while (m_isRunning)
	{
		SOCKET hClient = GetServer().TryAccept();

		if (hClient != INVALID_SOCKET)
		{
			// ★ [과제 요건]: 소켓을 기반으로 고유 ID 생성 (소켓 핸들을 그대로 ID로 사용)
			ClientSession* newSession = new ClientSession();
			newSession->id = static_cast<int>(hClient);
			newSession->name = "Unknown";
			newSession->signal = new NetSignal(hClient);

			RegisterSession(newSession);

			// 접속한 클라이언트에게 ID 할당 (서버 -> 클라)
			PacketAssignID assignPkt;
			PackingHelper::Packing_AssignID(assignPkt, newSession->id);
			newSession->signal->TrySend(assignPkt.header);

			// 대상 Sinal로부터 recv하는 쓰레드 생성
			std::thread recvThread(&ServerManager::RecvThread, this, newSession);
			recvThread.detach();
		}
	}
}

void ServerManager::RecvThread(ClientSession* session)
{
	while (m_isRunning)
	{
		// NetRunner 기반 콜백으로 패킷 처리
		int recvBytes = session->signal->TryRecv([this, session](char* packetData) {

			PacketHeader* header = (PacketHeader*)packetData;

			switch (header->type)
			{
			case PacketType::ON_CONNECT:
			{
				std::string name;
				if (PackingHelper::Unpack_OnConnect(packetData, name))
				{
					// 클라이언트가 보낸 이름 저장
					session->name = name;
				}
				break;
			}
			case PacketType::REQUEST_CHAT:
			{
				std::string chatMsg;
				if (PackingHelper::Unpack_RequestChat(packetData, chatMsg))
				{
					// ★ [과제 요건]: 서버가 클라이언트 이름과 메시지를 합쳐서 브로드캐스트
					PacketBroadcastChat broadcastPkt;
					if (PackingHelper::Packing_BroadcastChat(broadcastPkt, session->name, chatMsg))
					{
						Broadcast(broadcastPkt.header);
					}
				}
				break;
			}
			default:
				break;
			}

			});

		// 통신 종료 또는 오류
		if (recvBytes <= 0) { break; }
	}

	// 연결 종료 또는 오류 발생 시 해당 세션 제거
	UnregisterSession(session);
	delete session->signal;
	delete session;
}

void ServerManager::Broadcast(const PacketHeader& packet)
{
	EnterCriticalSection(&m_sessionsLock);

	// 모든 대상에 패킷 전송 시도
	for (ClientSession* session : m_sessions)
	{
		if (session != nullptr && session->signal != nullptr)
		{
			session->signal->TrySend(packet);
		}
	}

	LeaveCriticalSection(&m_sessionsLock);
}

void ServerManager::RegisterSession(ClientSession* newSession)
{
	EnterCriticalSection(&m_sessionsLock);
	m_sessions.push_back(newSession);
	LeaveCriticalSection(&m_sessionsLock);
}

void ServerManager::UnregisterSession(ClientSession* session)
{
	EnterCriticalSection(&m_sessionsLock);
	m_sessions.erase(std::remove(m_sessions.begin(), m_sessions.end(), session), m_sessions.end());
	LeaveCriticalSection(&m_sessionsLock);
}