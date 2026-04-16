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

void TextureManager::BuildDescriptorHeaps(ID3D12Device* device)
{
    // 텍스처 개수만큼 SRV 힙을 생성합니다. 현재 4개.
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 4;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));
}

void TextureManager::BuildShaderResourceViews(ID3D12Device* device)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    // 맵에서 텍스처 리소스를 가져옵니다.
    auto grassTex = mTextures.at("grassTex")->Resource;
    auto waterTex = mTextures.at("waterTex")->Resource;
    auto fenceTex = mTextures.at("fenceTex")->Resource;
    auto treeArrayTex = mTextures.at("treeArrayTex")->Resource;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = grassTex->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;
    device->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

    // 물 텍스처 SRV
    hDescriptor.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    srvDesc.Format = waterTex->GetDesc().Format;
    device->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

    // 철조망 텍스처 SRV
    hDescriptor.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    srvDesc.Format = fenceTex->GetDesc().Format;
    device->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);

    // 나무 텍스처 SRV (Texture2DArray 이므로 ViewDimension 변경 필요)
    hDescriptor.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    auto desc = treeArrayTex->GetDesc();
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Format = treeArrayTex->GetDesc().Format;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = -1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = treeArrayTex->GetDesc().DepthOrArraySize;
    device->CreateShaderResourceView(treeArrayTex.Get(), &srvDesc, hDescriptor);
}

const std::unordered_map<std::string, std::unique_ptr<Texture>>& TextureManager::GetTextures() const
{
    return mTextures;
}