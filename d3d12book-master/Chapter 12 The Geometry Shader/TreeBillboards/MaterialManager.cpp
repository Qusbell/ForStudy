#include "MaterialManager.h"

void MaterialManager::BuildMaterials()
{
    // [변경됨] 기존의 길고 반복되던 하드코딩 재질 생성 로직을 
    // 단수형 메서드(BuildMaterial)의 반복 호출로 리팩토링하여 가독성을 높였습니다.
    BuildMaterial("grass", 0, 0, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f), 0.125f);
    BuildMaterial("water", 1, 1, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f), 0.0f);
    BuildMaterial("wirefence", 2, 2, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f), 0.25f);
    BuildMaterial("treeArray", 3, 3, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f), 0.125f);
}


// [새로 추가됨] 매개변수를 기반으로 Material 객체를 생성하고 mMaterials 맵에 등록합니다.
void MaterialManager::BuildMaterial(const std::string& name, int matCBIndex, int diffuseSrvHeapIndex,
    DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness)
{
    auto mat = std::make_unique<Material>();
    mat->Name = name;
    mat->MatCBIndex = matCBIndex;
    mat->DiffuseSrvHeapIndex = diffuseSrvHeapIndex;
    mat->DiffuseAlbedo = diffuseAlbedo;
    mat->FresnelR0 = fresnelR0;
    mat->Roughness = roughness;

    mMaterials[name] = std::move(mat);
}


const std::unordered_map<std::string, std::unique_ptr<Material>>& MaterialManager::GetMaterials() const
{
    return mMaterials;
}

Material* MaterialManager::Get(const std::string& name) const
{
    auto it = mMaterials.find(name);
    if (it != mMaterials.end())
        return it->second.get();

    return nullptr;
}