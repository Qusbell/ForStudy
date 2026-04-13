// NetworkBase에서 이미 선언 중
// #include <winsock2.h>
// #include <windows.h>

#include "ServerBase.h"
#include "ServerManager.h"
#include <iostream> // 콘솔 출력용

int main()
{
	ServerManager serverManager;
	if (serverManager.TryStart(DEFAULT_PORT) == NetInitResult::Complete)
	{
		std::cout << "========================================" << std::endl;
		std::cout << " [System] 채팅 서버가 시작되었습니다." << std::endl;
		std::cout << " [System] 통신 포트 : " << DEFAULT_PORT << std::endl;
		std::cout << " [System] 서버를 종료하려면 창을 닫으세요." << std::endl;
		std::cout << "========================================" << std::endl;

		// 메인 쓰레드 계속 유지
		while (serverManager.IsRunning())
		{
			// 빈 루프를 방치하면 CPU 100% 점유율을 차지하므로, 
			// 100ms 단위로 쓰레드를 대기 상태로 전환시킴 (Release 모드 최적화 삭제 방지)
			Sleep(100);
		}
	}
	else
	{
		std::cout << "서버 시작에 실패했습니다." << std::endl;
	}

	return 0;
}