#include "ServerManager.h"

ServerManager::ServerManager(unsigned short port)
{
	m_server = new ServerBase(port);
}

ServerManager::~ServerManager()
{
	delete m_server;

	// NetSignal 객체들도 모두 삭제
	for (NetSignal* signal : m_signals)
	{
		if (signal != nullptr)
		{ delete signal; }
	}
}


NetInitResult ServerManager::TryStart()
{
	auto result = GetServer().NetInitialize();




	return result;
}
