#include "TextureManager.h"
#include "../../Common/DDSTextureLoader.h"

void TextureManager::LoadTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    auto grassTex = std::make_unique<Texture>();
    grassTex->Name = "grassTex";
    grassTex->Filename = L"../../Textures/grass.dds";
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
        cmdList, grassTex->Filename.c_str(),
        grassTex->Resource, grassTex->UploadHeap));

    auto waterTex = std::make_unique<Texture>();
    waterTex->Name = "waterTex";
    waterTex->Filename = L"../../Textures/water1.dds";
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
        cmdList, waterTex->Filename.c_str(),
        waterTex->Resource, waterTex->UploadHeap));

    auto fenceTex = std::make_unique<Texture>();
    fenceTex->Name = "fenceTex";
    fenceTex->Filename = L"../../Textures/WireFence.dds";
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
        cmdList, fenceTex->Filename.c_str(),
        fenceTex->Resource, fenceTex->UploadHeap));

    auto treeArrayTex = std::make_unique<Texture>();
    treeArrayTex->Name = "treeArrayTex";
    treeArrayTex->Filename = L"../../Textures/treeArray2.dds";
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
        cmdList, treeArrayTex->Filename.c_str(),
        treeArrayTex->Resource, treeArrayTex->UploadHeap));

    mTextures[grassTex->Name] = std::move(grassTex);
    mTextures[waterTex->Name] = std::move(waterTex);
    mTextures[fenceTex->Name] = std::move(fenceTex);
    mTextures[treeArrayTex->Name] = std::move(treeArrayTex);
}

const std::unordered_map<std::string, std::unique_ptr<Texture>>& TextureManager::GetTextures() const
{
    return mTextures;
}