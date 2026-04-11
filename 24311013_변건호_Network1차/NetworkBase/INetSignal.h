#pragma once
#include <winsock2.h>


class INetSignal
{
public:
	virtual ~INetSignal() = default;

protected:

	// @param buffer: 전송할 데이터 버퍼
	// @return : 실제로 전송된 바이트 수. 음수는 오류
	virtual int TrySend(const char* buffer) = 0;

	// @param buffer: 수신된 데이터를 저장할 버퍼
	// @return : 실제로 수신된 바이트 수. 0은 연결 종료, 음수는 오류
	virtual int TryRecv(char* buffer) = 0;
};