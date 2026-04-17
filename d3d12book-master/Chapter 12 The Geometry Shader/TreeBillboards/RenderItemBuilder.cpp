#include "RenderItemBuilder.h"

void RenderItemBuilder::Build(
    MaterialManager* matManager,
    const std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
    std::vector<std::unique_ptr<RenderItem>>& outAllRitems,
    std::vector<RenderItem*>* outRitemLayer,
    RenderItem*& outWavesRitem)
{
    UINT objCBIndex = 0;

    // 1. 물결(Waves) 아이템
    auto wavesRitem = std::make_unique<RenderItem>();
    wavesRitem->World = MathHelper::Identity4x4();
    DirectX::XMStoreFloat4x4(&wavesRitem->TexTransform, DirectX::XMMatrixScaling(5.0f, 5.0f, 1.0f));
    wavesRitem->ObjCBIndex = objCBIndex++;
    wavesRitem->Mat = matManager->Get("water");
    wavesRitem->Geo = geometries.at("waterGeo").get();
    wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
    wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
    wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

    outWavesRitem = wavesRitem.get(); // 메인 App의 애니메이션 업데이트용 포인터 저장
    outRitemLayer[(int)RenderLayer::Transparent].push_back(wavesRitem.get());
    outAllRitems.push_back(std::move(wavesRitem));

    // 2. 지형(Grid/Land) 아이템
    auto gridRitem = std::make_unique<RenderItem>();
    gridRitem->World = MathHelper::Identity4x4();
    DirectX::XMStoreFloat4x4(&gridRitem->TexTransform, DirectX::XMMatrixScaling(5.0f, 5.0f, 1.0f));
    gridRitem->ObjCBIndex = objCBIndex++;
    gridRitem->Mat = matManager->Get("grass");
    gridRitem->Geo = geometries.at("landGeo").get();
    gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
    gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
    gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

    outRitemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());
    outAllRitems.push_back(std::move(gridRitem));

    // 3. 박스(상자) 아이템
    auto boxRitem = std::make_unique<RenderItem>();
    DirectX::XMStoreFloat4x4(&boxRitem->World, DirectX::XMMatrixTranslation(3.0f, 2.0f, -9.0f));
    boxRitem->ObjCBIndex = objCBIndex++;
    boxRitem->Mat = matManager->Get("wirefence");
    boxRitem->Geo = geometries.at("boxGeo").get();
    boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
    boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
    boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

    outRitemLayer[(int)RenderLayer::AlphaTested].push_back(boxRitem.get());
    outAllRitems.push_back(std::move(boxRitem));

    // 4. 나무 빌보드(Tree Sprites) 아이템
    auto treeSpritesRitem = std::make_unique<RenderItem>();
    treeSpritesRitem->World = MathHelper::Identity4x4();
    treeSpritesRitem->ObjCBIndex = objCBIndex++;
    treeSpritesRitem->Mat = matManager->Get("treeArray");
    treeSpritesRitem->Geo = geometries.at("treeSpritesGeo").get();
    // HLSL 지오메트리 셰이더(Geometry Shader)가 Point List를 요구하므로 D3D_PRIMITIVE_TOPOLOGY_POINTLIST를 지정합니다.
    treeSpritesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    treeSpritesRitem->IndexCount = treeSpritesRitem->Geo->DrawArgs["points"].IndexCount;
    treeSpritesRitem->StartIndexLocation = treeSpritesRitem->Geo->DrawArgs["points"].StartIndexLocation;
    treeSpritesRitem->BaseVertexLocation = treeSpritesRitem->Geo->DrawArgs["points"].BaseVertexLocation;

    outRitemLayer[(int)RenderLayer::AlphaTestedTreeSprites].push_back(treeSpritesRitem.get());
    outAllRitems.push_back(std::move(treeSpritesRitem));
}