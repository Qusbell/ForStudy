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

    void Initialize(ID3D12Device* device);

    // 루트 시그니처 (보통 앱당 1개를 공유하므로 그대로 둡니다)
    void BuildRootSignature();
    ID3D12RootSignature* GetRootSignature() const { return mRootSignature.Get(); }

    // ------------------------------------------------------------------
    // [단수형 팩토리 메서드] 동적으로 객체를 추가하는 기능
    // ------------------------------------------------------------------
    void CompileShader(std::string name, std::wstring filename, const D3D_SHADER_MACRO* defines, std::string entrypoint, std::string target);
    void AddInputLayout(std::string name, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout);
    void BuildPSO(std::string name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);

    // ------------------------------------------------------------------
    // [하드코딩 테스트용 일괄 처리 메서드] 내부적으로 위 단수형 메서드를 호출
    // ------------------------------------------------------------------
    void BuildInputLayouts();
    void BuildShaders();
    void BuildPSOs(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool m4xMsaaState, UINT m4xMsaaQuality);

    // ------------------------------------------------------------------
    // Getter 메서드들
    // ------------------------------------------------------------------
    std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout(const std::string& name) { return mInputLayouts[name]; }
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>>& GetShaders() { return mShaders; }
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>>& GetPSOs() { return mPSOs; }

private:
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
    ID3D12Device* md3dDevice = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

    // 개별 변수 대신 Map으로 통합하여 유연하게 관리
    std::unordered_map<std::string, std::vector<D3D12_INPUT_ELEMENT_DESC>> mInputLayouts;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
};