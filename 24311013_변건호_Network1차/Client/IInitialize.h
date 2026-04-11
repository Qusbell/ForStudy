#pragma once


// 데이터 측면에서 초기화 가능한 개체에 대한 interface
class IInitialize
{
public:
	virtual ~IInitialize() = default;

	virtual int Initialize() = 0;
};
