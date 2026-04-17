#include "MaterialManager.h"

void MaterialManager::BuildMaterials()
{
    auto grass = std::make_unique<Material>();
    grass->Name = "grass";
    grass->MatCBIndex = 0;
    grass->DiffuseSrvHeapIndex = 0;
    grass->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    grass->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
    grass->Roughness = 0.125f;

    auto water = std::make_unique<Material>();
    water->Name = "water";
    water->MatCBIndex = 1;
    water->DiffuseSrvHeapIndex = 1;
    water->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
    water->FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
    water->Roughness = 0.0f;

    auto wirefence = std::make_unique<Material>();
    wirefence->Name = "wirefence";
    wirefence->MatCBIndex = 2;
    wirefence->DiffuseSrvHeapIndex = 2;
    wirefence->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    wirefence->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
    wirefence->Roughness = 0.25f;

    auto treeArray = std::make_unique<Material>();
    treeArray->Name = "treeArray";
    treeArray->MatCBIndex = 3;
    treeArray->DiffuseSrvHeapIndex = 3;
    treeArray->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    treeArray->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
    treeArray->Roughness = 0.125f;

    mMaterials["grass"] = std::move(grass);
    mMaterials["water"] = std::move(water);
    mMaterials["wirefence"] = std::move(wirefence);
    mMaterials["treeArray"] = std::move(treeArray);
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