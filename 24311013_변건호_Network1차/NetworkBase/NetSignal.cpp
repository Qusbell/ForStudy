#include "NetSignal.h"

NetSignal::NetSignal(SOCKET hSocket) : m_hSocket(hSocket)
{}


int NetSignal::TrySend(const char* buffer)
{
    int dataSize = (int)strlen(buffer) + 1; // 널 문자 포함

    // 1. 헤더(크기) 보내기
	int sentBytes = send(m_hSocket, (char*)&dataSize, sizeof(int), 0);
    if (sentBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

    // 2. 실제 데이터 보내기
	sentBytes = send(m_hSocket, buffer, dataSize, 0);
	if (sentBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

    return dataSize;
}

int NetSignal::TryRecv(char* buffer)
{
    int dataSize = 0;

    // 1. 헤더(크기) 먼저 받기
	int recvBytes = recv(m_hSocket, (char*)&dataSize, sizeof(int), 0);
    if (recvBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

    // 2. 받은 크기만큼 실제 데이터 받기
	recvBytes = recv(m_hSocket, buffer, dataSize, 0);
    if (recvBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

	return recvBytes;
}