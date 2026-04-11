#include "ClientBase.h"


ClientBase::ClientBase(const std::string& ip, unsigned short port)
	: NetworkBase(),
	m_serverIp(ip),
	m_serverPort(port)
{}

NetInitResult ClientBase::TryConnect()
{
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_serverPort); // 포트 번호 할당

	// IP 주소 변환 (문자열 -> 네트워크 바이트 순서)
	if (inet_pton(AF_INET, m_serverIp.c_str(), &serverAddr.sin_addr) <= 0)
	{
		// IP 주소 형식이 잘못되었을 때의 처리
		return NetInitResult::Fail_Connect;
	}

	// 서버로 연결 시도
	// 연결이 성공하거나 실패할 때까지 대기
	if (connect(GetSocket(), (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return NetInitResult::Fail_Connect;
	}

	return NetInitResult::Complete;
}


NetInitResult ClientBase::NetInitialize()
{
	// 1. 부모 클래스의 초기화 호출
	NetInitResult result = NetworkBase::NetInitialize();
	if (result != NetInitResult::Complete) { return result; }

	// 2. 서버로 연결 시도
	result = TryConnect();
	if (result != NetInitResult::Complete) { return result; }
	return result;
}
