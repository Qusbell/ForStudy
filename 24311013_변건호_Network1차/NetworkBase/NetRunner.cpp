#include "NetRunner.h"
#include <cstring>

NetRunner::NetRunner() : m_remainSize(0)
{
	// 버퍼 초기화
	memset(m_recvBuffer, 0, sizeof(m_recvBuffer));
}


int NetRunner::TrySend(const SOCKET target, const PacketHeader& packet)
{
	// 작성해주신 C++ 메모리 연속성 기법:
	// 패킷 구조체의 맨 앞이 PacketHeader이므로,
	// 시작 주소(&packet)부터 패킷 전체 크기(packet.size)만큼 전송하면 올바르게 전체가 복사됩니다.
	int sentBytes = send(target, (const char*)&packet, packet.size, 0);

	if (sentBytes == SOCKET_ERROR) { return SOCKET_ERROR; }

	return sentBytes;
}


int NetRunner::TryRecv(const SOCKET target, std::function<void(char*)> onPacketReceived)
{
	// 1. 남은 공간에 데이터 수신 (자신의 멤버 변수 버퍼 활용)
	int recvSize = recv(target, m_recvBuffer + m_remainSize, sizeof(m_recvBuffer) - m_remainSize, 0);

	// 0(연결종료) 또는 SOCKET_ERROR 반환
	if (recvSize <= 0) { return recvSize; }

	m_remainSize += recvSize;

	// 2. 버퍼에 패킷 헤더 크기 이상의 데이터가 있다면 조립/검사 시작
	while (m_remainSize >= sizeof(PacketHeader))
	{
		PacketHeader* header = (PacketHeader*)m_recvBuffer;

		// 매직 넘버 검증 (잘못된 데이터면 SOCKET_ERROR 반환하여 연결 강제 종료 유도)
		if (header->magic != PACKET_MAGIC) { return SOCKET_ERROR; }

		// 패킷 전체 크기만큼 다 안 들어왔다면 다음 recv를 위해 루프 대기
		if (m_remainSize < header->size) { break; }

		// 3. 완전한 패킷 1개가 조립됨 -> 콜백 실행
		onPacketReceived(m_recvBuffer);

		// 4. 처리한 패킷만큼 버퍼를 앞으로 당기고 남은 크기 갱신
		memmove(m_recvBuffer, m_recvBuffer + header->size, m_remainSize - header->size);
		m_remainSize -= header->size;
	}

	return recvSize;
}