#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::Initialize(ID3D12Device* device)
{
    md3dDevice = device;
}

void ShaderManager::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER slotRootParameter[4];
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsConstantBufferView(0);
    slotRootParameter[2].InitAsConstantBufferView(1);
    slotRootParameter[3].InitAsConstantBufferView(2);

    auto staticSamplers = GetStaticSamplers();

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
        (UINT)staticSamplers.size(), staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(md3dDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

// ----------------------------------------------------------------------
// 단수형 팩토리 메서드 구현
// ----------------------------------------------------------------------
void ShaderManager::CompileShader(std::string name, std::wstring filename, const D3D_SHADER_MACRO* defines, std::string entrypoint, std::string target)
{
    mShaders[name] = d3dUtil::CompileShader(filename.c_str(), defines, entrypoint, target);
}

void ShaderManager::AddInputLayout(std::string name, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout)
{
    mInputLayouts[name] = inputLayout;
}

void ShaderManager::BuildPSO(std::string name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs[name])));
}

// ----------------------------------------------------------------------
// 래퍼 메서드: 내부적으로 단수형 메서드를 호출
// ----------------------------------------------------------------------
void ShaderManager::BuildInputLayouts()
{
    AddInputLayout("std", {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        });

    AddInputLayout("treeSprite", {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        });
}

void ShaderManager::BuildShaders()
{
    const D3D_SHADER_MACRO defines[] = { "FOG", "1", NULL, NULL };
    const D3D_SHADER_MACRO alphaTestDefines[] = { "FOG", "1", "ALPHA_TEST", "1", NULL, NULL };

    CompileShader("standardVS", L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
    CompileShader("opaquePS", L"Shaders\\Default.hlsl", defines, "PS", "ps_5_0");
    CompileShader("alphaTestedPS", L"Shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_0");

    CompileShader("treeSpriteVS", L"Shaders\\TreeSprite.hlsl", nullptr, "VS", "vs_5_0");
    CompileShader("treeSpriteGS", L"Shaders\\TreeSprite.hlsl", nullptr, "GS", "gs_5_0");
    CompileShader("treeSpritePS", L"Shaders\\TreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_0");
}

void ShaderManager::BuildPSOs(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool m4xMsaaState, UINT m4xMsaaQuality)
{
    // 1. Opaque PSO 빌드
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
    ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    opaquePsoDesc.InputLayout = { mInputLayouts["std"].data(), (UINT)mInputLayouts["std"].size() };
    opaquePsoDesc.pRootSignature = mRootSignature.Get();
    opaquePsoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), mShaders["standardVS"]->GetBufferSize() };
    opaquePsoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()), mShaders["opaquePS"]->GetBufferSize() };
    opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    opaquePsoDesc.SampleMask = UINT_MAX;
    opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    opaquePsoDesc.NumRenderTargets = 1;
    opaquePsoDesc.RTVFormats[0] = backBufferFormat;
    opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    opaquePsoDesc.DSVFormat = depthStencilFormat;

    BuildPSO("opaque", opaquePsoDesc);

    // 2. Transparent PSO 빌드
    D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;
    D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
    transparencyBlendDesc.BlendEnable = true;
    transparencyBlendDesc.LogicOpEnable = false;
    transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
    transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
    transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;

    BuildPSO("transparent", transparentPsoDesc);

    // 3. Alpha Tested PSO 빌드
    D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPsoDesc = opaquePsoDesc;
    alphaTestedPsoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["alphaTestedPS"]->GetBufferPointer()), mShaders["alphaTestedPS"]->GetBufferSize() };
    alphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

    BuildPSO("alphaTested", alphaTestedPsoDesc);

    // 4. Tree Sprites PSO 빌드
    D3D12_GRAPHICS_PIPELINE_STATE_DESC treeSpritePsoDesc = opaquePsoDesc;
    treeSpritePsoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["treeSpriteVS"]->GetBufferPointer()), mShaders["treeSpriteVS"]->GetBufferSize() };
    treeSpritePsoDesc.GS = { reinterpret_cast<BYTE*>(mShaders["treeSpriteGS"]->GetBufferPointer()), mShaders["treeSpriteGS"]->GetBufferSize() };
    treeSpritePsoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["treeSpritePS"]->GetBufferPointer()), mShaders["treeSpritePS"]->GetBufferSize() };
    treeSpritePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    treeSpritePsoDesc.InputLayout = { mInputLayouts["treeSprite"].data(), (UINT)mInputLayouts["treeSprite"].size() };
    treeSpritePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

    BuildPSO("treeSprites", treeSpritePsoDesc);
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> ShaderManager::GetStaticSamplers()
{
    // (기존과 동일하므로 생략하지 않고 그대로 유지)
    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
        0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);

    const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
        1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);

    const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
        3, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
        4, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 8);

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
        5, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 8);

    return { pointWrap, pointClamp, linearWrap, linearClamp, anisotropicWrap, anisotropicClamp };
}