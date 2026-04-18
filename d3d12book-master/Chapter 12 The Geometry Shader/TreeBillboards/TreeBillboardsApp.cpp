//***************************************************************************************
// TreeBillboardsApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************
#include "TreeBillboardsApp.h"
#include "../../Common/MathHelper.h" // 혹시 모를 인식 오류 방지용 명시적 추가
#include "../../Common/Camera.h"     // 혹시 모를 인식 오류 방지용 명시적 추가

// 다른 곳에서도 참조할 수 있도록 extern 처리
extern const int gNumFrameResources = 3;

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

    // 메인 윈도우에 메뉴 부착
    HMENU hMenu = LoadMenu(mhAppInst, MAKEINTRESOURCE(IDR_MAIN_MENU));
    SetMenu(mhMainWnd, hMenu);

    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // [수정됨] 카메라 초기 위치 설정
    mCamera.SetPosition(0.0f, 15.0f, -50.0f);

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

    // [수정됨] 카메라의 렌즈 투영 행렬 세팅 업데이트
    mCamera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
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

    if (mCurrentMode == ToolMode::Camera)
    {
        SetCapture(mhMainWnd); // 카메라 모드일 때만 드래그 캡처
    }
    else if (mCurrentMode == ToolMode::PlantTree && (btnState & MK_LBUTTON))
    {
        Pick(x, y); // [추가] 마우스 위치로 광선을 쏴서 나무 심기 호출
    }
    else if (mCurrentMode == ToolMode::BuildFence && (btnState & MK_LBUTTON))
    {
        // Pick(x, y); // 펜스 짓기 구현 시 사용
    }
}

void TreeBillboardsApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    if (mCurrentMode == ToolMode::Camera)
    {
        ReleaseCapture();
    }
}

void TreeBillboardsApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    // [수정됨] 카메라 모드일 때만 Camera 객체를 통한 회전 허용
    if (mCurrentMode == ToolMode::Camera)
    {
        if ((btnState & MK_LBUTTON) != 0)
        {
            // Make each pixel correspond to a quarter of a degree.
            float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
            float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

            mCamera.Pitch(dy);
            mCamera.RotateY(dx);
        }
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

LRESULT TreeBillboardsApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_MODE_CAMERA:
            mCurrentMode = ToolMode::Camera;
            break;
        case ID_MODE_PLANT_TREE:
            mCurrentMode = ToolMode::PlantTree;
            break;
        case ID_MODE_BUILD_FENCE:
            mCurrentMode = ToolMode::BuildFence;
            break;
        case ID_FILE_SAVE:
            // TODO: (Step 4) 맵 저장 로직
            break;
        case ID_FILE_LOAD:
            // TODO: (Step 4) 맵 로드 로직
            break;
        case ID_ENV_DAY:
            // [수정됨] 낮 모드 적용
            SetEnvironmentMode(EnvironmentMode::Day);
            break;
        case ID_ENV_NIGHT:
            // [수정됨] 밤 모드 적용
            SetEnvironmentMode(EnvironmentMode::Night);
            break;
        }
        return 0; // 메시지 처리 완료
    }
    }

    // 기본 처리는 부모 클래스(D3DApp)로 넘김
    return D3DApp::MsgProc(hwnd, msg, wParam, lParam);
}

void TreeBillboardsApp::SetEnvironmentMode(EnvironmentMode mode)
{
    mEnvMode = mode;

    if (mEnvMode == EnvironmentMode::Day)
    {
        // 낮: 밝은 파란색 하늘
        mClearColor = { 0.2f, 0.694f, 0.996f, 1.0f };
    }
    else
    {
        // 밤: 어두운 남색/검은색 계열 하늘
        mClearColor = { 0.05f, 0.05f, 0.15f, 1.0f };
    }
}

void TreeBillboardsApp::OnKeyboardInput(const GameTimer& gt)
{
    const float dt = gt.DeltaTime();
	const float cameraSpeed = 80.0f; // 카메라 이동 속도

    // [수정됨] 카메라 모드일 때 WASD 이동 제어 추가
    if (mCurrentMode == ToolMode::Camera)
    {
        if (GetAsyncKeyState('W') & 0x8000) { mCamera.Walk(cameraSpeed * dt); }
        if (GetAsyncKeyState('S') & 0x8000) { mCamera.Walk(-cameraSpeed * dt); }
        if (GetAsyncKeyState('A') & 0x8000) { mCamera.Strafe(-cameraSpeed * dt); }
        if (GetAsyncKeyState('D') & 0x8000) { mCamera.Strafe(cameraSpeed * dt); }
    }
}

void TreeBillboardsApp::UpdateCamera(const GameTimer& gt)
{
    // [수정됨] 구면 좌표계 연산을 제거하고 Camera 객체의 뷰 행렬 업데이트 호출
    mCamera.UpdateViewMatrix();
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
    // [수정됨] Camera 객체에서 View, Proj 행렬 및 위치를 가져옴
    XMMATRIX view = mCamera.GetView();
    XMMATRIX proj = mCamera.GetProj();

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

    // [수정됨] mEyePos 대신 Camera 객체의 위치 사용
    mMainPassCB.EyePosW = mCamera.GetPosition3f();

    mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
    mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = gt.TotalTime();
    mMainPassCB.DeltaTime = gt.DeltaTime();

    // [수정됨] 현재 환경 모드(낮/밤)에 따라 조명(Lighting) 강도와 방향을 다르게 설정합니다.
    if (mEnvMode == EnvironmentMode::Day)
    {
        // 낮: 기존과 동일하게 밝은 태양빛과 주변광
        mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
        mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
        mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
        mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
        mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
        mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
        mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
    }
    else
    {
        // 밤: 어둡고 푸르스름한 달빛 느낌
        mMainPassCB.AmbientLight = { 0.05f, 0.05f, 0.1f, 1.0f }; // 아주 어두운 주변광

        // 메인 조명(Lights[0])을 달빛으로 취급하여 방향과 색상 변경
        mMainPassCB.Lights[0].Direction = { -0.57735f, -0.3f, 0.57735f };
        mMainPassCB.Lights[0].Strength = { 0.1f, 0.1f, 0.3f }; // 푸르고 약한 빛

        // 보조 조명들은 밤이므로 끕니다 (Strength를 0으로)
        mMainPassCB.Lights[1].Strength = { 0.0f, 0.0f, 0.0f };
        mMainPassCB.Lights[2].Strength = { 0.0f, 0.0f, 0.0f };
    }

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


void TreeBillboardsApp::Pick(int sx, int sy)
{
    // 1. 화면 픽셀 좌표를 NDC 공간([-1, 1])으로 변환
    // [수정됨] XMMATRIX 대신 XMFLOAT4X4를 반환하는 GetProj4x4f()를 사용하여 요소에 접근합니다.
    float vx = (+2.0f * sx / mClientWidth - 1.0f) / mCamera.GetProj4x4f()(0, 0);
    float vy = (-2.0f * sy / mClientHeight + 1.0f) / mCamera.GetProj4x4f()(1, 1);

    // 2. View 공간에서의 Ray(광선) 정의
    XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

    // 3. View 공간의 Ray를 World 공간으로 변환
    XMMATRIX V = mCamera.GetView();
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

    rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
    rayDir = XMVector3TransformNormal(rayDir, invView);
    rayDir = XMVector3Normalize(rayDir);

    // 4. Ray와 지형 평면(y=0 기준)의 교차점 근사 계산
    float dirY = XMVectorGetY(rayDir);
    if (abs(dirY) < 0.001f) return; // Ray가 수평(지형과 평행)이면 무시

    float t = -XMVectorGetY(rayOrigin) / dirY;
    if (t < 0.0f) return; // 교차점이 카메라 뒤편이면 무시

    XMVECTOR hitPos = rayOrigin + t * rayDir;
    float hitX = XMVectorGetX(hitPos);
    float hitZ = XMVectorGetZ(hitPos);

    // 지형의 범위(예: 50x50 이라면 -25 ~ 25)를 벗어났는지 확인
	const float terrainHalfSize = 60.0f; // 지형의 절반 크기
    if (hitX < -terrainHalfSize || hitX > terrainHalfSize || hitZ < -terrainHalfSize || hitZ > terrainHalfSize) { return; }

    // [반영됨] 작성하신 Hills 네임스페이스를 활용하여 실제 지형의 높이(Y)를 가져옴
    float hitY = Hills::GetHeight(hitX, hitZ);

    // 5. 현재 선택된 툴 모드에 따라 오브젝트 배치
    if (mCurrentMode == ToolMode::PlantTree)
    {
        PlantTreeAt(hitX, hitY, hitZ);
    }
}

// 2. PlantTreeAt 함수가 아주 깔끔해집니다.
void TreeBillboardsApp::PlantTreeAt(float x, float y, float z)
{
    // 1. 데이터 추가 위임
    mResourceManager.AddTree(x, y, z);

    // 2. GPU 동기화 및 커맨드 리스트 준비
    FlushCommandQueue();
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    // 3. 리소스 매니저에게 버퍼 업데이트 위임
    mResourceManager.UpdateTreeGeometryBuffer(mCommandList.Get());

    // 4. 커맨드 큐 실행 및 대기
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    FlushCommandQueue();

    // 5. 렌더 아이템 매니저의 Draw 카운트 갱신
    auto& treeRitems = mRenderItemManager.GetRitemLayer(RenderLayer::AlphaTestedTreeSprites);
    for (auto& e : treeRitems)
    {
        e->IndexCount = mResourceManager.GetTreeCount();
    }
}
