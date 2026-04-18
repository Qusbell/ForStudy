//***************************************************************************************
// TreeBillboardsApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "../../Common/d3dApp.h"
#include "../../Common/MathHelper.h"
#include "../../Common/UploadBuffer.h"
#include "../../Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "Waves.h"

// 분리된 매니저 클래스 헤더
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "RenderItemManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

// 다른 곳에서도 참조할 수 있도록 extern 처리
extern const int gNumFrameResources = 3;

class TreeBillboardsApp : public D3DApp
{
public:
    TreeBillboardsApp(HINSTANCE hInstance);
    TreeBillboardsApp(const TreeBillboardsApp& rhs) = delete;
    TreeBillboardsApp& operator=(const TreeBillboardsApp& rhs) = delete;
    ~TreeBillboardsApp();

    virtual bool Initialize()override;

private:
    virtual void OnResize()override;
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;

    virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

    void OnKeyboardInput(const GameTimer& gt);
    void UpdateCamera(const GameTimer& gt);
    void AnimateMaterials(const GameTimer& gt);
    void UpdateObjectCBs(const GameTimer& gt);
    void UpdateMaterialCBs(const GameTimer& gt);
    void UpdateMainPassCB(const GameTimer& gt);
    void UpdateWaves(const GameTimer& gt);

    void BuildDescriptorHeaps();
    void BuildFrameResources();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

private:

    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    UINT mCbvSrvDescriptorSize = 0;

    ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

    // 분리된 매니저 클래스들
    ResourceManager mResourceManager;
    ShaderManager mShaderManager;
    RenderItemManager mRenderItemManager;

    std::unique_ptr<Waves> mWaves;

    PassConstants mMainPassCB;

    XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV2 - 0.1f;
    float mRadius = 50.0f;

    POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        TreeBillboardsApp theApp(hInstance);
        if (!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

TreeBillboardsApp::TreeBillboardsApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
{
}

TreeBillboardsApp::~TreeBillboardsApp()
{
    if (md3dDevice != nullptr)
        FlushCommandQueue();
}

bool TreeBillboardsApp::Initialize()
{
    if (!D3DApp::Initialize())
        return false;

    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

    // 매니저 초기화
    mResourceManager.Initialize(md3dDevice.Get());
    mShaderManager.Initialize(md3dDevice.Get());
    mRenderItemManager.Initialize(&mResourceManager);

    // 리소스 및 셰이더 로드
    mResourceManager.LoadTextures(mCommandList.Get());

    mShaderManager.BuildRootSignature();
    mShaderManager.BuildInputLayouts();

    BuildDescriptorHeaps();

    mShaderManager.BuildShaders();

    // 지오메트리 로드
    mResourceManager.BuildBoxGeometry(mCommandList.Get());
    mResourceManager.BuildLandGeometry(mCommandList.Get());
    mResourceManager.BuildWavesGeometry(mCommandList.Get(), mWaves.get());
    mResourceManager.BuildTreeSpritesGeometry(mCommandList.Get());

    // 매터리얼 로드
    mResourceManager.BuildMaterials();

    // 렌더 아이템 세팅
    mRenderItemManager.BuildRenderItems();

    BuildFrameResources();

    // 파이프라인 스테이트(PSO) 세팅
    mShaderManager.BuildPSOs(mBackBufferFormat, mDepthStencilFormat, m4xMsaaState, m4xMsaaQuality);

    // Execute the initialization commands.
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();

    return true;
}

void TreeBillboardsApp::OnResize()
{
    D3DApp::OnResize();

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void TreeBillboardsApp::Update(const GameTimer& gt)
{
    OnKeyboardInput(gt);
    UpdateCamera(gt);

    // Cycle through the circular frame resource array.
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }

    AnimateMaterials(gt);
    UpdateObjectCBs(gt);
    UpdateMaterialCBs(gt);
    UpdateMainPassCB(gt);
    UpdateWaves(gt);
}

void TreeBillboardsApp::Draw(const GameTimer& gt)
{
    auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;
    auto& psos = mShaderManager.GetPSOs();

    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(cmdListAlloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), psos["opaque"].Get()));

    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), (float*)&mMainPassCB.FogColor, 0, nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
    mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    mCommandList->SetGraphicsRootSignature(mShaderManager.GetRootSignature());

    auto passCB = mCurrFrameResource->PassCB->Resource();
    mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

    DrawRenderItems(mCommandList.Get(), mRenderItemManager.GetRitemLayer(RenderLayer::Opaque));

    mCommandList->SetPipelineState(psos["alphaTested"].Get());
    DrawRenderItems(mCommandList.Get(), mRenderItemManager.GetRitemLayer(RenderLayer::AlphaTested));

    mCommandList->SetPipelineState(psos["treeSprites"].Get());
    DrawRenderItems(mCommandList.Get(), mRenderItemManager.GetRitemLayer(RenderLayer::AlphaTestedTreeSprites));

    mCommandList->SetPipelineState(psos["transparent"].Get());
    DrawRenderItems(mCommandList.Get(), mRenderItemManager.GetRitemLayer(RenderLayer::Transparent));

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // Done recording commands.
    ThrowIfFailed(mCommandList->Close());

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Swap the back and front buffers
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    // Advance the fence value to mark commands up to this fence point.
    mCurrFrameResource->Fence = ++mCurrentFence;

    // Add an instruction to the command queue to set a new fence point. 
    // Because we are on the GPU timeline, the new fence point won't be 
    // set until the GPU finishes processing all the commands prior to this Signal().
    mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void TreeBillboardsApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture(mhMainWnd);
}

void TreeBillboardsApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void TreeBillboardsApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.
        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // Make each pixel correspond to 0.2 unit in the scene.
        float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
        float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);

        // Update the camera radius based on input.
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void TreeBillboardsApp::OnKeyboardInput(const GameTimer& gt)
{
}

void TreeBillboardsApp::UpdateCamera(const GameTimer& gt)
{
    // Convert Spherical to Cartesian coordinates.
    mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
    mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
    mEyePos.y = mRadius * cosf(mPhi);

    // Build the view matrix.
    XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, view);
}

void TreeBillboardsApp::AnimateMaterials(const GameTimer& gt)
{
    auto& Mats = mResourceManager.GetMaterials();

    // Scroll the water material texture coordinates.
    auto waterMat = Mats["water"].get();

    float& tu = waterMat->MatTransform(3, 0);
    float& tv = waterMat->MatTransform(3, 1);

    tu += 0.1f * gt.DeltaTime();
    tv += 0.02f * gt.DeltaTime();

    if (tu >= 1.0f)
        tu -= 1.0f;

    if (tv >= 1.0f)
        tv -= 1.0f;

    waterMat->MatTransform(3, 0) = tu;
    waterMat->MatTransform(3, 1) = tv;

    // Material has changed, so need to update cbuffer.
    waterMat->NumFramesDirty = gNumFrameResources;
}

void TreeBillboardsApp::UpdateObjectCBs(const GameTimer& gt)
{
    auto currObjectCB = mCurrFrameResource->ObjectCB.get();
    for (auto& e : mRenderItemManager.GetAllRitems())
    {
        // Only update the cbuffer data if the constants have changed.  
        // This needs to be tracked per frame resource.
        if (e->NumFramesDirty > 0)
        {
            XMMATRIX world = XMLoadFloat4x4(&e->World);
            XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
            XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

            currObjectCB->CopyData(e->ObjCBIndex, objConstants);

            // Next FrameResource need to be updated too.
            e->NumFramesDirty--;
        }
    }
}

void TreeBillboardsApp::UpdateMaterialCBs(const GameTimer& gt)
{
    auto& Mats = mResourceManager.GetMaterials();

    auto currMaterialCB = mCurrFrameResource->MaterialCB.get();
    for (auto& e : Mats)
    {
        // Only update the cbuffer data if the constants have changed.  If the cbuffer
        // data changes, it needs to be updated for each FrameResource.
        Material* mat = e.second.get();
        if (mat->NumFramesDirty > 0)
        {
            XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

            MaterialConstants matConstants;
            matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
            matConstants.FresnelR0 = mat->FresnelR0;
            matConstants.Roughness = mat->Roughness;
            XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

            currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

            // Next FrameResource need to be updated too.
            mat->NumFramesDirty--;
        }
    }
}

void TreeBillboardsApp::UpdateMainPassCB(const GameTimer& gt)
{
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    mMainPassCB.EyePosW = mEyePos;
    mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
    mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = gt.TotalTime();
    mMainPassCB.DeltaTime = gt.DeltaTime();
    mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

    auto currPassCB = mCurrFrameResource->PassCB.get();
    currPassCB->CopyData(0, mMainPassCB);
}

void TreeBillboardsApp::UpdateWaves(const GameTimer& gt)
{
    // Every quarter second, generate a random wave.
    static float t_base = 0.0f;
    if ((mTimer.TotalTime() - t_base) >= 0.25f)
    {
        t_base += 0.25f;

        int i = MathHelper::Rand(4, mWaves->RowCount() - 5);
        int j = MathHelper::Rand(4, mWaves->ColumnCount() - 5);

        float r = MathHelper::RandF(0.2f, 0.5f);

        mWaves->Disturb(i, j, r);
    }

    // Update the wave simulation.
    mWaves->Update(gt.DeltaTime());

    // Update the wave vertex buffer with the new solution.
    auto currWavesVB = mCurrFrameResource->WavesVB.get();
    for (int i = 0; i < mWaves->VertexCount(); ++i)
    {
        Vertex v;

        v.Pos = mWaves->Position(i);
        v.Normal = mWaves->Normal(i);

        // Derive tex-coords from position by 
        // mapping [-w/2,w/2] --> [0,1]
        v.TexC.x = 0.5f + v.Pos.x / mWaves->Width();
        v.TexC.y = 0.5f - v.Pos.z / mWaves->Depth();

        currWavesVB->CopyData(i, v);
    }

    // Set the dynamic VB of the wave renderitem to the current frame VB.
    mRenderItemManager.GetWavesRitem()->Geo->VertexBufferGPU = currWavesVB->Resource();
}

void TreeBillboardsApp::BuildDescriptorHeaps()
{
    //
    // Create the SRV heap.
    //
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 4;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

    //
    // Fill out the heap with actual descriptors.
    //
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    auto& textures = mResourceManager.GetTextures();
    auto grassTex = textures["grassTex"]->Resource;
    auto waterTex = textures["waterTex"]->Resource;
    auto fenceTex = textures["fenceTex"]->Resource;
    auto treeArrayTex = textures["treeArrayTex"]->Resource;
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = grassTex->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;
    md3dDevice->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

    // next descriptor
    hDescriptor.Offset(1, mCbvSrvDescriptorSize);

    srvDesc.Format = waterTex->GetDesc().Format;
    md3dDevice->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

    // next descriptor
    hDescriptor.Offset(1, mCbvSrvDescriptorSize);

    srvDesc.Format = fenceTex->GetDesc().Format;
    md3dDevice->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);

    // next descriptor
    hDescriptor.Offset(1, mCbvSrvDescriptorSize);

    auto desc = treeArrayTex->GetDesc();
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Format = treeArrayTex->GetDesc().Format;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = -1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = treeArrayTex->GetDesc().DepthOrArraySize;
    md3dDevice->CreateShaderResourceView(treeArrayTex.Get(), &srvDesc, hDescriptor);
}

void TreeBillboardsApp::BuildFrameResources()
{
    for (int i = 0; i < gNumFrameResources; ++i)
    {
        auto& Mats = mResourceManager.GetMaterials();

        mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
            1, (UINT)mRenderItemManager.GetAllRitems().size(), (UINT)Mats.size(), mWaves->VertexCount()));
    }
}

void TreeBillboardsApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
    UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
    UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

    auto objectCB = mCurrFrameResource->ObjectCB->Resource();
    auto matCB = mCurrFrameResource->MaterialCB->Resource();

    // For each render item...
    for (size_t i = 0; i < ritems.size(); ++i)
    {
        auto ri = ritems[i];

        cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
        cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
        cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

        CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
        D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

        cmdList->SetGraphicsRootDescriptorTable(0, tex);
        cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
        cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

        cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
    }
}