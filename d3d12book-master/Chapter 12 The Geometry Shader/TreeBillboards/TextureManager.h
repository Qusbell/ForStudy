#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>

// ==============================================================================
// [Refactoring Phase 2-1] 텍스처 로드 로직을 전담하는 매니저 클래스
// 메인 App의 텍스처 로딩 및 컨테이너 소유권을 이관받습니다.
// ==============================================================================
class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager() = default;

    // 디바이스와 커맨드 리스트만 전달받아 텍스처를 로드합니다.
    void LoadTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    // 렌더링 파이프라인(SRV 생성 등)에서 참조할 수 있도록 텍스처 맵을 반환합니다.
    const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
};