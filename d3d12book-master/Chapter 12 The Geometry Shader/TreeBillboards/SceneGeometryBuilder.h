#pragma once

#include "../../Common/d3dUtil.h"
#include "../../Common/GeometryGenerator.h"
#include <memory>

// ==============================================================================
// [Refactoring Phase 3-1] Shape 지오메트리 생성 분리
// 100줄이 넘어가는 지오메트리 버퍼 생성 로직들을 모듈화하기 위한 빌더 클래스
// ==============================================================================
class SceneGeometryBuilder
{
public:
    // 박스, 그리드, 구, 원기둥을 병합한 형태의 지오메트리 버퍼를 생성하여 반환합니다.
    static std::unique_ptr<MeshGeometry> BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};