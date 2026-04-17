#pragma once

#include "../../Common/d3dUtil.h"
#include "RenderItem.h"
#include "RenderLayer.h"
#include "MaterialManager.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

// ==============================================================================
// [Refactoring Phase 5-1] 렌더 아이템(씬 구성 객체) 조립 분리
// 메인 App의 BuildRenderItems() 함수를 캡슐화합니다.
// ==============================================================================
class RenderItemBuilder
{
public:
    // 지오메트리와 머티리얼 매니저를 참조하여 RenderItem을 생성하고,
    // 메인 App이 관리하는 리스트(AllRitems, RitemLayer)에 할당합니다.
    static void Build(
        MaterialManager* matManager,
        const std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
        std::vector<std::unique_ptr<RenderItem>>& outAllRitems,
        std::vector<RenderItem*>* outRitemLayer,
        RenderItem*& outWavesRitem
    );
};