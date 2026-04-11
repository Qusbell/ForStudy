#pragma once
#include "NetInitResult.h"

// Net상에서의 초기화에 대한 인터페이스
class INetInitialize
{
public:
	virtual ~INetInitialize() = default;

	virtual NetInitResult NetInitialize() = 0;
};
