#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

// ==============================================================================
// [Refactoring Phase 4-1] 루트 시그니처 및 셰이더 관리 매니저
// ==============================================================================
class PipelineManager
{
public:
    PipelineManager() = default;
    ~PipelineManager() = default;

    // 루트 시그니처를 생성합니다.
    void BuildRootSignature(ID3D12Device* device);

    // 셰이더를 컴파일하고 입력 레이아웃(Input Layout)을 정의합니다.
    void BuildShadersAndInputLayout();

    // Getters
    ID3D12RootSignature* GetRootSignature() const { return mRootSignature.Get(); }
    ID3DBlob* GetShader(const std::string& name) const;
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const { return mInputLayout; }
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetTreeSpriteInputLayout() const { return mTreeSpriteInputLayout; }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;
};