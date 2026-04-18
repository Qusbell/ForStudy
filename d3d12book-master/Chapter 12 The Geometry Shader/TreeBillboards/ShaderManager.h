#pragma once

#include "../../Common/d3dUtil.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <string>

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    // 디바이스 초기화 주입
    void Initialize(ID3D12Device* device);

    // 루트 시그니처 및 인풋 레이아웃 생성 메서드
    void BuildRootSignature();
    void BuildInputLayouts();
    void BuildShaders();

    // Step 1-6: PSO 생성 메서드 추가 (포맷과 MSAA 관련 값들을 매개변수로 받음)
    void BuildPSOs(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool m4xMsaaState, UINT m4xMsaaQuality);

    // 외부(App)에서 접근하기 위한 Getter
    ID3D12RootSignature* GetRootSignature() const { return mRootSignature.Get(); }
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetStdInputLayout() const { return mStdInputLayout; }
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetTreeSpriteInputLayout() const { return mTreeSpriteInputLayout; }

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>>& GetShaders() { return mShaders; }

    // Step 1-6: PSO 맵 Getter 추가
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>>& GetPSOs() { return mPSOs; }

private:
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
    ID3D12Device* md3dDevice = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mStdInputLayout;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;

    // Step 1-6: 기존 App에서 분리된 PSO 데이터 변수
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
};