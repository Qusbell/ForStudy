#pragma once

#include "../../Common/d3dUtil.h"
#include <memory>

// ==============================================================================
// [Refactoring Phase 3-1] Box 지오메트리 생성 로직 단독 분리
// 메인 App의 BuildBoxGeometry() 함수를 캡슐화합니다.
// ==============================================================================
class SceneGeometryBuilder
{
public:
    // 디바이스와 커맨드 리스트를 받아 Box 지오메트리를 생성 후 반환합니다.
    static std::unique_ptr<MeshGeometry> BuildBoxGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};