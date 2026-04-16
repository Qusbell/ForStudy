#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>

// ==============================================================================
// [Refactoring Phase 2-2] SRV(Shader Resource View) 생성 로직 통합
// ==============================================================================
class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager() = default;

    // 1. 텍스처 데이터를 메모리에 로드합니다.
    void LoadTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    // 2. 텍스처를 위한 Descriptor Heap을 생성합니다. (SRV 용)
    void BuildDescriptorHeaps(ID3D12Device* device);

    // 3. 로드된 텍스처들의 SRV를 Heap에 생성(View 만들기)합니다.
    void BuildShaderResourceViews(ID3D12Device* device);

    // Getter
    const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() const;
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return mSrvDescriptorHeap.Get(); }

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

    // 텍스처 SRV들을 담아둘 서술자 힙
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
};