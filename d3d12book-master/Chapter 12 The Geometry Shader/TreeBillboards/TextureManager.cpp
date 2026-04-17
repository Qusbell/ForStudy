#include "TextureManager.h"
#include "../../Common/DDSTextureLoader.h"

// [변경됨] 생성자에서 매개변수로 받은 device를 멤버 변수 mDevice에 저장합니다.
TextureManager::TextureManager(ID3D12Device* device)
    : mDevice(device)
{
}

TextureManager::~TextureManager()
{
}


// 매개변수를 기반으로 Texture 객체를 할당하고, DDS 파일을 로드하여 mTextures 맵에 보관합니다.
void TextureManager::LoadTexture(const std::string& name, const std::wstring& filename, ID3D12GraphicsCommandList* cmdList)
{
    if (mTextures.find(name) != mTextures.end()) { return; }

    auto tex = std::make_unique<Texture>();
    tex->Name = name;
    tex->Filename = filename;

    // [변경됨] 멤버 변수인 mDevice를 사용합니다.
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(mDevice.Get(), cmdList, tex->Filename.c_str(), tex->Resource, tex->UploadHeap));

    // [새로 추가됨] 맵은 순서를 보장하지 않으므로, 로드된 순서를 vector에 따로 기록합니다.
    mTextureLoadOrder.push_back(tex->Name);
    mTextures[tex->Name] = std::move(tex);
}

// 원본 코드의 SRV 디스크립터 힙 빌드 부분
void TextureManager::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    // [변경됨] 하드코딩된 4 대신 로드된 텍스처의 개수만큼 동적으로 크기를 할당합니다.
    srvHeapDesc.NumDescriptors = static_cast<UINT>(mTextures.size());
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    // [변경됨] 멤버 변수인 mDevice를 사용합니다.
    ThrowIfFailed(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    // [변경됨] 로드된 순서가 담긴 vector를 순회(foreach)하여 SRV를 자동 생성합니다.
    for (const auto& texName : mTextureLoadOrder)
    {
        BuildShaderResourceView(texName, hDescriptor);
    }
}

// 단일 텍스처에 대한 SRV를 생성하고, 디스크립터 핸들을 다음 위치로 오프셋시킵니다.
// [변경됨] dimension 매개변수를 없애고 내부에서 리소스의 속성을 읽어 자동 판별합니다.
void TextureManager::BuildShaderResourceView(const std::string& name, CD3DX12_CPU_DESCRIPTOR_HANDLE& hDescriptor)
{
    auto resource = mTextures[name]->Resource;
    D3D12_RESOURCE_DESC desc = resource->GetDesc(); // [새로 추가됨] 리소스 속성 가져오기

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.Format;

    // [새로 추가됨] ArraySize(배열 크기)가 1보다 크면 자동으로 Texture2DArray로 판별합니다.
    if (desc.DepthOrArraySize > 1)
    {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
    }
    else
    {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }

    // [변경됨] 멤버 변수인 mDevice를 사용합니다.
    mDevice->CreateShaderResourceView(resource.Get(), &srvDesc, hDescriptor);

    // 디스크립터 핸들을 다음 위치로 이동시킵니다.
    hDescriptor.Offset(1, mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
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