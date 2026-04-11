#include "NetworkBase.h"

SOCKET NetworkBase::GetSocket() const
{
	return m_hSocket;
}

NetworkBase::NetworkBase() :
	m_wsaData({ 0 }),
	m_hSocket(INVALID_SOCKET),
	m_wsaInitFlag(false)
{}

NetworkBase::~NetworkBase()
{
	// 1. 소켓 닫기
	TryCloseSocket();

	// 2. 윈속 종료 처리
	TryWSACleanUp();
}

NetInitResult NetworkBase::InitializeWSA()
{
	// 윈속 생성
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);

	// 실패 체크
	if (result != 0) { return NetInitResult::Fail_WSA; }

	// 윈속 Init 성공 플래그
	m_wsaInitFlag = true;
	return NetInitResult::Complete;
}


NetInitResult NetworkBase::InitializeSocket()
{
	// TCP 소켓 생성 (IPv4, 스트림, TCP)
	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);

	// 실패 체크
	if (m_hSocket == INVALID_SOCKET) { return NetInitResult::Fail_Socket; }

	return NetInitResult::Complete;
}

void NetworkBase::TryCloseSocket()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

void NetworkBase::TryWSACleanUp()
{
	if (m_wsaInitFlag)
	{
		WSACleanup();
		m_wsaInitFlag = false;
	}
}

NetInitResult NetworkBase::NetInitialize()
{
	// 1. 윈속 초기화
	NetInitResult result = InitializeWSA();
	if (result != NetInitResult::Complete) { return result; }

	// 2. 소켓 생성
	result = InitializeSocket();
	if (result != NetInitResult::Complete)
	{
		TryWSACleanUp();
		return result;
	}

	return result;
}

int NetworkBase::Send(const char* buffer, int len)
{
	int result = send(m_hSocket, buffer, len, 0);
	return result;
}

int NetworkBase::Recv(char* buffer, int len)
{
	int result = recv(m_hSocket, buffer, len, 0);
	return result;
}
