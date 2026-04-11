#pragma once

enum class NetInitResult
{
	Complete = 1,

	//=== NetworkBase ===//
	// 서버 / 클라 공통

	// WSA 초기화 실패
	Fail_WSA = -1,
	// 소켓 생성 실패
	Fail_Socket = -2,

	//=== Server 에러 ===//

	// 소켓에 IP 주소와 포트 번호 할당 실패
	Fail_Bind = -3,
	// 클라이언트의 연결 요청을 수신 대기하는 상태로 전환 실패
	Fail_Listen = -4,


	//=== Client 에러 ===//

	// 서버로 연결 실패 (IP 주소 형식 오류, 연결 실패 등)
	Fail_Connect = -5
};
