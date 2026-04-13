#include "NetSignal.h"

NetSignal::NetSignal(SOCKET hSocket) : m_hSocket(hSocket), m_runner()
{
}

NetSignal::~NetSignal()
{
    if (m_hSocket != INVALID_SOCKET)
    {
        closesocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
}

int NetSignal::TrySend(const PacketHeader& packet)
{
    // NetRunner에게 전송 위임 (메모리 연속성을 활용하여 전송)
    return m_runner.TrySend(m_hSocket, packet);
}

int NetSignal::TryRecv(std::function<void(char*)> onPacketReceived)
{
    // NetRunner에게 수신 위임 (패킷 조립 완료 시 콜백 호출)
    return m_runner.TryRecv(m_hSocket, onPacketReceived);
}