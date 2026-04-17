#pragma once

#include "../../Common/d3dUtil.h"
#include <memory>

// ==============================================================================
// [Refactoring Phase 3] 지오메트리 생성 로직 분리
// 메인 App의 Build*Geometry() 함수들을 캡슐화합니다.
// ==============================================================================
class SceneGeometryBuilder
{
public:
    // [Phase 3-1] Box 지오메트리를 생성하여 반환합니다.
    static std::unique_ptr<MeshGeometry> BuildBoxGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    // [Phase 3-2] Land(지형) 지오메트리를 생성하여 반환합니다.
    static std::unique_ptr<MeshGeometry> BuildLandGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};