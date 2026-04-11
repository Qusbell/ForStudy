#include "ServerBase.h"
#include <iostream>

ServerBase::ServerBase(unsigned short port) : NetworkBase(), m_port(port)
{}

NetInitResult ServerBase::TryBind()
{
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP로부터의 접속 허용
	serverAddr.sin_port = htons(m_port);            // 사용할 포트 번호 할당

	// Bind (소켓에 IP 주소와 포트 번호 할당)
	if (bind(GetSocket(), (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return NetInitResult::Fail_Bind;
	}

	return NetInitResult::Complete;
}

NetInitResult ServerBase::TryListen()
{
	// Listen (클라이언트의 접속 요청 대기 상태로 전환)
	// SOMAXCONN <-- 수용가능 인원수(Max)
	if (listen(GetSocket(), SOMAXCONN) == SOCKET_ERROR)
	{
		return NetInitResult::Fail_Listen;
	}

	return NetInitResult::Complete;
}


NetInitResult ServerBase::NetInitialize()
{
	// 1. 부모 클래스의 초기화 호출 (WSAStartup, socket 생성)
	NetInitResult result = NetworkBase::NetInitialize();
	if (result != NetInitResult::Complete) { return result; }

	// 2. 바인드 시도
	result = TryBind();
	if (result != NetInitResult::Complete)
	{
		return result;
	}

	// 3. 리슨 시도
	result = TryListen();
	if (result != NetInitResult::Complete)
	{
		return result;
	}

	return result;
}

SOCKET ServerBase::TryAccept()
{
	SOCKADDR_IN clientAddr = { 0 };
	int clientAddrLen = sizeof(clientAddr);

	// 클라이언트 접속 수락 (동기 블로킹 방식)
	// 접속이 올 때까지 여기서 실행이 멈춥니다.
	SOCKET hClientSock = accept(GetSocket(), (SOCKADDR*)&clientAddr, &clientAddrLen);

	// 성공 시 클라이언트와 통신할 새로운 소켓 반환, 실패 시 INVALID_SOCKET 반환
	return hClientSock;
}