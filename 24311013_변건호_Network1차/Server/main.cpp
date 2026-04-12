// NetworkBase에서 이미 선언 중
// #include <winsock2.h>
// #include <windows.h>

#include "ServerBase.h"
#include "ServerManager.h"

int main()
{
	ServerManager serverManager;
	if (serverManager.TryStart(DEFAULT_PORT) == NetInitResult::Complete)
	{
		// 계속 유지
		while (serverManager.IsRunning()) {}
	}

	return 0;
}