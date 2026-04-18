#pragma once

#include "../../Common/d3dUtil.h"
#include <unordered_map>
#include <string>
#include <memory>

class Waves;

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void Initialize(ID3D12Device* device);

    // 테스트 용도로 복구된 하드코딩 일괄 로드 메서드
    void LoadTextures(ID3D12GraphicsCommandList* cmdList);

    // 단일 텍스처 로드 메서드
    void LoadTexture(ID3D12GraphicsCommandList* cmdList, std::string name, std::wstring filename);

    void BuildBoxGeometry(ID3D12GraphicsCommandList* cmdList);
    void BuildLandGeometry(ID3D12GraphicsCommandList* cmdList);
    void BuildWavesGeometry(ID3D12GraphicsCommandList* cmdList, Waves* waves);
    void BuildTreeSpritesGeometry(ID3D12GraphicsCommandList* cmdList);

    // 복수형 BuildMaterials()를 단수형 BuildMaterial(...)로 변경
    void BuildMaterial(std::string name, int matCBIndex, int diffuseSrvHeapIndex, DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness);

    void BuildMaterials();

    std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() { return mTextures; }
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GetGeometries() { return mGeometries; }

    // 1-3: 재질 맵에 접근하기 위한 Getter 메서드
    std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() { return mMaterials; }

private:
    float GetHillsHeight(float x, float z) const;
    DirectX::XMFLOAT3 GetHillsNormal(float x, float z) const;

private:
    ID3D12Device* md3dDevice = nullptr;

    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;

    // Step 1-3: TreeBillboardsApp.h에서 이동된 재질 리소스 맵
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
};