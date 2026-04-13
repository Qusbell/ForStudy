#include "ClientManager.h"
#include <string>

ClientManager::ClientManager(const HWND hMainWnd) :
	m_client(nullptr),
	m_hMainWnd(hMainWnd),
	m_signal(nullptr),
	m_isRunning(false)
{
	InitializeCriticalSection(&m_recvQueueLock);
}

ClientManager::~ClientManager()
{
	m_isRunning = false;

	if (m_client != nullptr)
	{
		delete m_client;
		m_client = nullptr;
	}

	if (m_signal != nullptr)
	{
		delete m_signal;
		m_signal = nullptr;
	}

	DeleteCriticalSection(&m_recvQueueLock);
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

		// ★ [과제 요건]: 서버에 접속 성공하면 자신의 이름을 패킷으로 전송
		std::string myName = "User_" + std::to_string((int)socket); // 임시로 소켓 번호를 이름으로 사용
		PacketOnConnect connectPkt;
		PackingHelper::Packing_OnConnect(connectPkt, myName);
		m_signal->TrySend(connectPkt.header);

		// recv 쓰레드 시작
		std::thread recvThread(&ClientManager::RecvThread, this);
		recvThread.detach();
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

const std::string ClientManager::GetRecvMessage()
{
	std::string message;

	EnterCriticalSection(&m_recvQueueLock);
	if (m_recvQueue.empty())
	{
		message = "";
	}
	else
	{
		message = m_recvQueue.front();
		m_recvQueue.pop();
	}
	LeaveCriticalSection(&m_recvQueueLock);

	return message;
}

void ClientManager::RecvThread()
{
	while (m_isRunning)
	{
		// NetRunner를 통해 패킷이 완전하게 조립되었을 때 실행될 람다(콜백)
		int recvBytes = m_signal->TryRecv([this](char* packetData) {

			PacketHeader* header = (PacketHeader*)packetData;

			switch (header->type)
			{
			case PacketType::ASSIGN_ID:
			{
				int myId = 0;
				if (PackingHelper::Unpack_AssignID(packetData, myId))
				{
					// ID 부여받음. 필요 시 콘솔 출력이나 UI 처리
				}
				break;
			}
			case PacketType::BROADCAST_CHAT:
			{
				std::string name, chat;
				if (PackingHelper::Unpack_BroadcastChat(packetData, name, chat))
				{
					// "[이름] 채팅내용" 형태로 포매팅
					std::string displayStr = "[" + name + "] " + chat;

					EnterCriticalSection(&m_recvQueueLock);
					m_recvQueue.push(displayStr);
					LeaveCriticalSection(&m_recvQueueLock);

					// UI 갱신 요청
					PostMessage(m_hMainWnd, WM_RECV_DATA, 0, (LPARAM)this);
				}
				break;
			}
			default:
				break;
			}
			});

		// 상대방이 종료했거나 오류 발생 시 루프 탈출
		if (recvBytes <= 0) { break; }
	}
}

void ClientManager::TrySendChat(const std::string& message)
{
	if (m_isRunning && m_signal != nullptr)
	{
		PacketRequestChat chatPkt;
		if (PackingHelper::Packeting_RequestChat(chatPkt, message))
		{
			// 패킷 헤더 주소를 넘기면 메모리 연속성으로 전체가 전송됨
			m_signal->TrySend(chatPkt.header);
		}
	}
}