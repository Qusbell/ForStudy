#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector> // [새로 추가됨] 순서를 기억하기 위해 vector 포함

class TextureManager
{
public:
    // [변경됨] 생성자에서 Device를 한 번만 주입받도록 수정
    TextureManager(ID3D12Device* device);
    ~TextureManager();

    // [변경됨] device 매개변수 제거
    void LoadTextures(ID3D12GraphicsCommandList* cmdList);

    // 단일 텍스처를 로드하고 생성하는 헬퍼 메서드
    void LoadTexture(const std::string& name, const std::wstring& filename, ID3D12GraphicsCommandList* cmdList);

    // 원본에 있던 디스크립터 힙 생성 메서드
    void BuildDescriptorHeaps();

    // 3. 로드된 텍스처들의 SRV를 Heap에 생성(View 만들기)합니다.
    // [변경됨] dimension 매개변수를 제거하고 내부에서 자동 판별하도록 수정
    void BuildShaderResourceView(const std::string& name, CD3DX12_CPU_DESCRIPTOR_HANDLE& hDescriptor);

    Texture* GetTexture(const std::string& name);
    const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() const;

    // SRV 힙 반환 메서드
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

    // [새로 추가됨] 텍스처가 로드된 순서를 저장하여 SRV 생성 시 순서를 보장합니다.
    std::vector<std::string> mTextureLoadOrder;

    // SRV 디스크립터 힙
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap;

    // Device 객체를 클래스 내부에 보관하여 재사용
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
};