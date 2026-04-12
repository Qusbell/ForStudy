#pragma once
#include <winsock2.h>
#include <string>


class INetSignal
{
public:
	virtual ~INetSignal() = default;

	// @param buffer: 전송할 데이터 버퍼
	// @return : 실제로 전송된 바이트 수. 음수는 오류
	virtual int TrySend(const std::string& buffer) = 0;



	virtual void TrySend(const PacketType type, const std::string& buffer) = 0;



	// @param buffer: 수신된 데이터를 저장할 버퍼
	// @return : 실제로 수신된 바이트 수. 0은 연결 종료, 음수는 오류
	virtual int TryRecv(std::string& buffer) = 0;
};