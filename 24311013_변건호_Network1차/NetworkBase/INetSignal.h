#pragma once

// 네트워크 버퍼 최대 크기
#define MAX_BUFFER_SIZE 1024

class INetSignal
{
public:
	virtual ~INetSignal() = default;

	// @param buffer: 전송할 데이터 버퍼
	// @param len: 전송할 데이터의 길이
	// @return: 실제로 전송된 바이트 수. 음수는 오류
	virtual int Send(const char* buffer, int len) = 0;

	// @param buffer: 수신된 데이터를 저장할 버퍼
	// @param len: 수신할 데이터의 최대 길이
	// @return: 실제로 수신된 바이트 수. 0은 연결 종료, 음수는 오류
	virtual int Recv(char* buffer, int len) = 0;
};