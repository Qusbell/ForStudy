#pragma once

#include "../../Common/d3dUtil.h"
#include "../../Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "Waves.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <array>

using namespace DirectX;

// [리팩토링] 기하학 데이터의 생성뿐만 아니라 생명주기(Lifetime)와 소유권을 관리하는 클래스로 변경
class GeometryManager
{
public:
    GeometryManager(ID3D12Device* device);
    ~GeometryManager() = default;

    // 복사 및 대입 방지 (리소스 관리자의 단일 소유권 보장)
    GeometryManager(const GeometryManager&) = delete;
    GeometryManager& operator=(const GeometryManager&) = delete;

    // [리팩토링] 모든 지오메트리를 한 번에 생성하고 맵에 보관합니다.
    void BuildAllGeometries(ID3D12GraphicsCommandList* cmdList, Waves* waves);

    // [리팩토링] 외부에서 지오메트리가 필요할 때 이름(string)으로 검색하여 포인터를 반환합니다.
    // 반환 타입이 원시 포인터(*)이므로 소유권은 GeometryManager가 계속 유지합니다.
    MeshGeometry* GetGeometry(const std::string& name) const;

    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> GetGeometries();

private:
    // 내부 헬퍼 함수들
    void BuildLandGeometry(ID3D12GraphicsCommandList* cmdList);
    void BuildWavesGeometry(ID3D12GraphicsCommandList* cmdList, Waves* waves);
    void BuildBoxGeometry(ID3D12GraphicsCommandList* cmdList);
    void BuildTreeSpritesGeometry(ID3D12GraphicsCommandList* cmdList);

private:
    ID3D12Device* md3dDevice = nullptr;

    // [리팩토링] 생성된 MeshGeometry 객체들의 실질적인 소유권(Ownership)을 가집니다.
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
};