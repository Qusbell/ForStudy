#include "TextureManager.h"
#include "../../Common/DDSTextureLoader.h"

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

void TextureManager::LoadTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // [변경됨] 개별적으로 길게 하드코딩되던 텍스처 로드 로직을 LoadTexture 메서드를 통해 간결화했습니다.
    LoadTexture("grassTex", L"../../Textures/grass.dds", device, cmdList);
    LoadTexture("waterTex", L"../../Textures/water1.dds", device, cmdList);
    LoadTexture("fenceTex", L"../../Textures/WireFence.dds", device, cmdList);
    LoadTexture("treeArrayTex", L"../../Textures/treeArray2.dds", device, cmdList);
}

// [새로 추가됨] 매개변수를 기반으로 Texture 객체를 할당하고, DDS 파일을 로드하여 mTextures 맵에 보관합니다.
void TextureManager::LoadTexture(const std::string& name, const std::wstring& filename, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    auto tex = std::make_unique<Texture>();
    tex->Name = name;
    tex->Filename = filename;
    
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device, cmdList, 
                                                      tex->Filename.c_str(), 
                                                      tex->Resource, 
                                                      tex->UploadHeap));

    mTextures[tex->Name] = std::move(tex);
}

// [복구됨] 원본 코드의 SRV 디스크립터 힙 빌드 부분 (변형 없이 그대로 합침)
void TextureManager::BuildDescriptorHeaps(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 4;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));
}

void TextureManager::BuildShaderResourceViews(ID3D12Device* device)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    auto grassTex = mTextures["grassTex"]->Resource;
    auto waterTex = mTextures["waterTex"]->Resource;
    auto fenceTex = mTextures["fenceTex"]->Resource;
    auto treeArrayTex = mTextures["treeArrayTex"]->Resource;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = grassTex->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = grassTex->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    device->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

    // waterTex
    hDescriptor.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    srvDesc.Format = waterTex->GetDesc().Format;
    srvDesc.Texture2D.MipLevels = waterTex->GetDesc().MipLevels;
    device->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

    // fenceTex
    hDescriptor.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    srvDesc.Format = fenceTex->GetDesc().Format;
    srvDesc.Texture2D.MipLevels = fenceTex->GetDesc().MipLevels;
    device->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);

    // treeArrayTex
    hDescriptor.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Format = treeArrayTex->GetDesc().Format;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = treeArrayTex->GetDesc().MipLevels;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = treeArrayTex->GetDesc().DepthOrArraySize;
    device->CreateShaderResourceView(treeArrayTex.Get(), &srvDesc, hDescriptor);
}

Texture* TextureManager::GetTexture(const std::string& name)
{
    auto it = mTextures.find(name);
    if (it != mTextures.end())
    {
        return it->second.get();
    }
    return nullptr;
}

const std::unordered_map<std::string, std::unique_ptr<Texture>>& TextureManager::GetTextures() const
{
    return mTextures;
}

ID3D12DescriptorHeap* TextureManager::GetSrvDescriptorHeap() const
{
    return mSrvDescriptorHeap.Get();
}