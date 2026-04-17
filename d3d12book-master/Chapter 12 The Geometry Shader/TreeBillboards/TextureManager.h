#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    void LoadTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    // [새로 추가됨] 단일 텍스처를 로드하고 생성하는 헬퍼 메서드
    void LoadTexture(const std::string& name, const std::wstring& filename, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    // [복구됨] 원본에 있던 디스크립터 힙 생성 메서드
    void BuildDescriptorHeaps(ID3D12Device* device);

    // 3. 로드된 텍스처들의 SRV를 Heap에 생성(View 만들기)합니다.
    void BuildShaderResourceViews(ID3D12Device* device);

    Texture* GetTexture(const std::string& name);
    const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() const;
    
    // [복구됨] SRV 힙 반환 메서드
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    
    // [복구됨] SRV 디스크립터 힙
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap;
};