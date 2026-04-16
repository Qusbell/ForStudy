#pragma once

// ==============================================================================
// [Refactoring] 렌더링 패스(Layer) 구분을 위한 열거형을 독립적인 헤더로 분리.
// 순수 데이터 타입이므로 다른 모듈을 참조하지 않습니다.
// ==============================================================================
enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	AlphaTestedTreeSprites,
	Count
};