#pragma once
#include "..\NetworkBase\NetworkBase.h"


class IServerInitialize
{
public:
	virtual ~IServerInitialize() = default;

protected:
	virtual NetInitResult TryBind() = 0;
	virtual NetInitResult TryListen() = 0;

public:
	virtual SOCKET TryAccept() = 0;
};