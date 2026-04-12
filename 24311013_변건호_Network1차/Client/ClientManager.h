#pragma once

#include "ClientBase.h"
#include <thread>



class ClientManager
{
private:
	// 클라이언트 객체
	ClientBase* m_client;
	inline ClientBase& GetClient() { return *m_client; }
	
	// 클라이언트가 실행 중인지 여부
	bool m_isRunning;

public:
	ClientManager(const std::string& ip, unsigned short port);
	~ClientManager();

	NetInitResult TryStart();

};

