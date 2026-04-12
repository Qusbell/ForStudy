#include "NetSignal.h"

NetSignal::NetSignal(SOCKET hSocket) : m_hSocket(hSocket)
{}

NetSignal::~NetSignal()
{
    if (m_hSocket != INVALID_SOCKET)
    {
        closesocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
	}
}

int NetSignal::TrySend(const std::string& buffer)
{
    // 문자열의 길이를 구합니다. (널 문자 제외)
    int dataSize = (int)buffer.length();

    // 1. 헤더(데이터 크기) 먼저 보내기
    int sentBytes = send(m_hSocket, (char*)&dataSize, sizeof(int), 0);
    if (sentBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

    // 2. 실제 문자열 데이터 보내기
    sentBytes = send(m_hSocket, buffer.c_str(), dataSize, 0);
    if (sentBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

    return dataSize;
}


int NetSignal::TryRecv(std::string& buffer)
{
    int dataSize = 0;

    // 1. 헤더(데이터 크기) 먼저 받기
    int recvBytes = recv(m_hSocket, (char*)&dataSize, sizeof(int), 0);
    // 상대방이 정상 종료했거나(0) 에러 발생 시 처리
    if (recvBytes == SOCKET_ERROR || recvBytes == 0) { return recvBytes; }

    // 2. 수신할 데이터 크기만큼 string 버퍼 공간 확보
    buffer.resize(dataSize);

    // 3. 확보된 string 메모리 공간에 직접 데이터 받기
    recvBytes = recv(m_hSocket, &buffer[0], dataSize, 0);
    if (recvBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

    return recvBytes;
}