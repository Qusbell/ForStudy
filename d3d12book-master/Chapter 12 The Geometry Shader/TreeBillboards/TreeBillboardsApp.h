#pragma once

#include "../../Common/d3dApp.h"
#include "../../Common/MathHelper.h"
#include "../../Common/UploadBuffer.h"
#include "../../Common/GeometryGenerator.h"
#include "../../Common/Camera.h" // Camera 헤더 추가
#include "FrameResource.h"
#include "Waves.h"

// 분리된 매니저 클래스 헤더
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "RenderItemManager.h"

 // 메뉴 리소스 ID 인클루드
#include "resource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

// 툴의 상태를 나타내는 열거형 선언
enum class ToolMode
{
    Camera,
    PlantTree,
    BuildFence
};

// [추가] 환경(낮/밤) 상태를 나타내는 열거형
enum class EnvironmentMode
{
    Day,
    Night
};


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

    // 메뉴 이벤트 처리를 위한 가상 함수 오버라이드
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    // [추가] 환경 모드 제어 함수
    void SetEnvironmentMode(EnvironmentMode mode);

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
    // [추가] 픽킹 및 오브젝트 배치 함수
    void Pick(int sx, int sy);
    void PlantTreeAt(float x, float y, float z);

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

    // [수정됨] 기존 구면 좌표계 및 행렬 변수를 Camera 객체로 대체
    Camera mCamera;

    // 현재 선택된 툴 모드 상태 변수
    ToolMode mCurrentMode = ToolMode::Camera;

    // [추가] 현재 환경 모드 및 배경색 변수
    EnvironmentMode mEnvMode = EnvironmentMode::Day;
    std::array<float, 4> mClearColor = { 0.2f, 0.694f, 0.996f, 1.0f }; // 기본: 낮(하늘색)


    POINT mLastMousePos;
};