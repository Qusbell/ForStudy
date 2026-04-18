#include "RenderItemManager.h"
#include "ResourceManager.h"

RenderItemManager::RenderItemManager()
{
}

RenderItemManager::~RenderItemManager()
{
}

void RenderItemManager::Initialize(ResourceManager* resMgr)
{
    mResourceManager = resMgr;
}

// 렌더 아이템 생성을 위한 범용 팩토리 메서드
RenderItem* RenderItemManager::BuildRenderItem(
    RenderLayer layer,
    std::string geoName,
    std::string submeshName,
    std::string matName,
    UINT objCBIndex,
    DirectX::XMFLOAT4X4 world,
    DirectX::XMFLOAT4X4 texTransform,
    D3D12_PRIMITIVE_TOPOLOGY primitiveType)
{
    auto& geometries = mResourceManager->GetGeometries();
    auto& materials = mResourceManager->GetMaterials();

    auto ritem = std::make_unique<RenderItem>();
    ritem->World = world;
    ritem->TexTransform = texTransform;
    ritem->ObjCBIndex = objCBIndex;
    ritem->Mat = materials[matName].get();
    ritem->Geo = geometries[geoName].get();
    ritem->PrimitiveType = primitiveType;
    ritem->IndexCount = ritem->Geo->DrawArgs[submeshName].IndexCount;
    ritem->StartIndexLocation = ritem->Geo->DrawArgs[submeshName].StartIndexLocation;
    ritem->BaseVertexLocation = ritem->Geo->DrawArgs[submeshName].BaseVertexLocation;

    // 포인터 저장용 변수
    RenderItem* ritemPtr = ritem.get();

    // 레이어 및 전체 리스트에 등록
    mRitemLayer[(int)layer].push_back(ritemPtr);
    mAllRitems.push_back(std::move(ritem));

    return ritemPtr;
}

// 테스트 용도: 단수형 메서드를 사용하여 기존 하드코딩 데이터 일괄 생성
void RenderItemManager::BuildRenderItems()
{
    DirectX::XMFLOAT4X4 identity = MathHelper::Identity4x4();

    DirectX::XMFLOAT4X4 texScale;
    DirectX::XMStoreFloat4x4(&texScale, DirectX::XMMatrixScaling(5.0f, 5.0f, 1.0f));

    DirectX::XMFLOAT4X4 boxWorld;
    DirectX::XMStoreFloat4x4(&boxWorld, DirectX::XMMatrixTranslation(3.0f, 2.0f, -9.0f));

    // 1. Water (Waves) - 파생 포인터를 유지해야 하므로 반환값을 mWavesRitem에 저장
    mWavesRitem = BuildRenderItem(RenderLayer::Transparent, "waterGeo", "grid", "water", 0, identity, texScale);

    // 2. Grid (Land)
    BuildRenderItem(RenderLayer::Opaque, "landGeo", "grid", "grass", 1, identity, texScale);

    // 3. Box
    BuildRenderItem(RenderLayer::AlphaTested, "boxGeo", "box", "wirefence", 2, boxWorld, identity);

    // 4. Tree Sprites
    BuildRenderItem(RenderLayer::AlphaTestedTreeSprites, "treeSpritesGeo", "points", "treeSprites", 3, identity, identity, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}