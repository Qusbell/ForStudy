#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>

// ==============================================================================
// [Refactoring Phase 2-3] 머티리얼(Material) 생성 및 관리를 전담하는 모듈
// 다른 매니저(TextureManager 등)를 직접 참조하지 않고 완전히 독립적으로 동작합니다.
// ==============================================================================
class MaterialManager
{
public:
    MaterialManager() = default;
    ~MaterialManager() = default;

    // 잔디, 물, 철조망, 나무 머티리얼을 생성합니다.
    void BuildMaterials();

    // [새로 추가됨] 단일 재질(Material)을 생성하는 단수형 헬퍼 메서드
    void BuildMaterial(const std::string& name, int matCBIndex, int diffuseSrvHeapIndex,
        DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness);

    // 전체 머티리얼 맵을 반환합니다 (업데이트 루프용)
    const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const;

    // 특정 이름의 머티리얼 포인터를 반환합니다 (RenderItem 조립용)
    Material* Get(const std::string& name) const;

private:
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
};