#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class Waves;

namespace Hills
{
    float GetHeight(float x, float z);

    DirectX::XMFLOAT3 GetNormal(float x, float z);
}

struct TreeSpriteVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Size;
};


class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void Initialize(ID3D12Device* device);

    // 테스트 용도: 단수형 메서드를 재사용하여 하드코딩된 텍스처들을 일괄 로드합니다.
    void LoadTextures(ID3D12GraphicsCommandList* cmdList);
    void LoadTexture(ID3D12GraphicsCommandList* cmdList, std::string name, std::wstring filename);

    // 지오메트리 생성을 위한 공용(Unified) 메서드
    void BuildGeometry(
        ID3D12GraphicsCommandList* cmdList,
        std::string geoName,
        std::string submeshName,
        const void* vertexData,
        UINT vertexByteStride,
        UINT vertexBufferByteSize,
        const std::vector<std::uint16_t>& indices);

    void BuildBoxGeometry(ID3D12GraphicsCommandList* cmdList);
    void BuildLandGeometry(ID3D12GraphicsCommandList* cmdList);
    void BuildWavesGeometry(ID3D12GraphicsCommandList* cmdList, Waves* waves);
    void BuildTreeSpritesGeometry(ID3D12GraphicsCommandList* cmdList);

    void AddTree(float x, float y, float z);
    void UpdateTreeGeometryBuffer(ID3D12GraphicsCommandList* cmdList);
    UINT GetTreeCount() const { return (UINT)mTreeVertices.size(); }

    // 테스트 용도: 단수형 메서드를 재사용하여 하드코딩된 재질들을 일괄 생성합니다.
    void BuildMaterials();
    void BuildMaterial(std::string name, int matCBIndex, int diffuseSrvHeapIndex, DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness);

    std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() { return mTextures; }
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GetGeometries() { return mGeometries; }
    std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() { return mMaterials; }

private:
    ID3D12Device* md3dDevice = nullptr;

    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

    // [추가됨] 동적으로 관리할 나무 데이터 (App 클래스에서 이쪽으로 이사 옴)
    std::vector<TreeSpriteVertex> mTreeVertices;
};