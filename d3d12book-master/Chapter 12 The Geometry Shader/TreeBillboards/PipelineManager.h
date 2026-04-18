#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

// ==============================================================================
// [Refactoring Phase 4] 루트 시그니처, 셰이더, PSO 파이프라인 관리 매니저
// ==============================================================================
class PipelineManager
{
public:
    PipelineManager() = default;
    ~PipelineManager() = default;

    void BuildRootSignature(ID3D12Device* device);
    void BuildShadersAndInputLayout();

    // [Phase 4-2 추가] PSO(파이프라인 상태 객체) 생성 로직
    // D3DApp이 가진 MSAA 상태와 포맷을 인자로 받아 외부 결합도를 없앱니다.
    void BuildPSOs(ID3D12Device* device, bool m4xMsaaState, UINT m4xMsaaQuality, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat);

    ID3D12RootSignature* GetRootSignature() const { return mRootSignature.Get(); }
    ID3D12PipelineState* GetPSO(const std::string& name) const;

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;
};