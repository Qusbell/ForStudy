#pragma once

#include "../../Common/d3dUtil.h"
#include "../../Common/MathHelper.h"
#include <vector>
#include <memory>

// TreeBillboardsApp.cpp에 정의된 전역 상수 참조
extern const int gNumFrameResources;

// 전방 선언 (ResourceManager 참조용)
class ResourceManager;

struct RenderItem
{
	RenderItem() = default;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	AlphaTestedTreeSprites,
	Count
};

class RenderItemManager
{
public:
	RenderItemManager();
	~RenderItemManager();

	// Step 1-8: 기존 App에서 옮겨온 전체 렌더 아이템 생성 메서드
	void BuildRenderItems(ResourceManager* resMgr);

	// 데이터에 접근하기 위한 Getter / Setter
	std::vector<std::unique_ptr<RenderItem>>& GetAllRitems() { return mAllRitems; }
	std::vector<RenderItem*>& GetRitemLayer(RenderLayer layer) { return mRitemLayer[(int)layer]; }

	RenderItem* GetWavesRitem() const { return mWavesRitem; }
	void SetWavesRitem(RenderItem* item) { mWavesRitem = item; }

private:
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];
	RenderItem* mWavesRitem = nullptr;
};