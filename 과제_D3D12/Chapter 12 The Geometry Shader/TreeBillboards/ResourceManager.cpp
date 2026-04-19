#include "ResourceManager.h"
#include "../../Common/GeometryGenerator.h"
#include "../../Common/MathHelper.h"
#include "FrameResource.h"
#include "Waves.h"
#include <array>


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize(ID3D12Device* device)
{
    md3dDevice = device;
}

void ResourceManager::LoadTextures(ID3D12GraphicsCommandList* cmdList)
{
    LoadTexture(cmdList, "woodCrateTex", L"../../Textures/WoodCrate01.dds");
    LoadTexture(cmdList, "grassTex", L"../../Textures/grass.dds");
    LoadTexture(cmdList, "waterTex", L"../../Textures/water1.dds");
    LoadTexture(cmdList, "fenceTex", L"../../Textures/WireFence.dds");
    LoadTexture(cmdList, "treeArrayTex", L"../../Textures/treeArray2.dds");
}

void ResourceManager::LoadTexture(ID3D12GraphicsCommandList* cmdList, std::string name, std::wstring filename)
{
    auto tex = std::make_unique<Texture>();
    tex->Name = name;
    tex->Filename = filename;
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice,
        cmdList, tex->Filename.c_str(),
        tex->Resource, tex->UploadHeap));

    mTextures[name] = std::move(tex);
}

// 중복된 버퍼 생성 로직을 통합한 공용 지오메트리 빌더
void ResourceManager::BuildGeometry(
    ID3D12GraphicsCommandList* cmdList,
    std::string geoName,
    std::string submeshName,
    const void* vertexData,
    UINT vertexByteStride,
    UINT vertexBufferByteSize,
    const std::vector<std::uint16_t>& indices)
{
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = geoName;

    // 동적 버퍼 등 vertexData가 nullptr인 경우 생성을 스킵
    if (vertexData != nullptr)
    {
        ThrowIfFailed(D3DCreateBlob(vertexBufferByteSize, &geo->VertexBufferCPU));
        CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertexData, vertexBufferByteSize);

        geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
            cmdList, vertexData, vertexBufferByteSize, geo->VertexBufferUploader);
    }
    else
    {
        geo->VertexBufferCPU = nullptr;
        geo->VertexBufferGPU = nullptr;
    }

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = vertexByteStride;
    geo->VertexBufferByteSize = vertexBufferByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    geo->DrawArgs[submeshName] = submesh;

    mGeometries[geoName] = std::move(geo);
}

void ResourceManager::BuildBoxGeometry(ID3D12GraphicsCommandList* cmdList)
{
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);

    std::vector<Vertex> vertices(box.Vertices.size());
    for (size_t i = 0; i < box.Vertices.size(); ++i)
    {
        vertices[i].Pos = box.Vertices[i].Position;
        vertices[i].Normal = box.Vertices[i].Normal;
        vertices[i].TexC = box.Vertices[i].TexC;

		vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // 흰색
    }

    std::vector<std::uint16_t> indices = box.GetIndices16();
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

    // 통합 빌더 호출
    BuildGeometry(cmdList, "boxGeo", "box", vertices.data(), sizeof(Vertex), vbByteSize, indices);
}

void ResourceManager::BuildLandGeometry(ID3D12GraphicsCommandList* cmdList)
{
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

    std::vector<Vertex> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        auto& p = grid.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Pos.y = Hills::GetHeight(p.x, p.z);
        vertices[i].Normal = Hills::GetNormal(p.x, p.z);
        vertices[i].TexC = grid.Vertices[i].TexC;

        float height = vertices[i].Pos.y;
        if (height < -10.0f)      vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f); // 모래색
        else if (height < 5.0f)   vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f); // 연두색 (풀)
        else if (height < 12.0f)  vertices[i].Color = XMFLOAT4(0.10f, 0.48f, 0.19f, 1.0f); // 짙은 녹색 (숲)
        else if (height < 20.0f)  vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f); // 갈색 (바위)
        else                      vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);    // 흰색 (눈)
    }

    std::vector<std::uint16_t> indices = grid.GetIndices16();
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

    // 통합 빌더 호출
    BuildGeometry(cmdList, "landGeo", "grid", vertices.data(), sizeof(Vertex), vbByteSize, indices);
}

void ResourceManager::BuildWavesGeometry(ID3D12GraphicsCommandList* cmdList, Waves* waves)
{
    std::vector<std::uint16_t> indices(3 * waves->TriangleCount());
    assert(waves->VertexCount() < 0x0000ffff);

    int m = waves->RowCount();
    int n = waves->ColumnCount();
    int k = 0;
    for (int i = 0; i < m - 1; ++i)
    {
        for (int j = 0; j < n - 1; ++j)
        {
            indices[k] = i * n + j;
            indices[k + 1] = i * n + j + 1;
            indices[k + 2] = (i + 1) * n + j;

            indices[k + 3] = (i + 1) * n + j;
            indices[k + 4] = i * n + j + 1;
            indices[k + 5] = (i + 1) * n + j + 1;

            k += 6;
        }
    }

    UINT vbByteSize = waves->VertexCount() * sizeof(Vertex);

    // 동적 업데이트 용도이므로 데이터 포인터로 nullptr 전달
    BuildGeometry(cmdList, "waterGeo", "grid", nullptr, sizeof(Vertex), vbByteSize, indices);
}

void ResourceManager::BuildTreeSpritesGeometry(ID3D12GraphicsCommandList* cmdList)
{
    // [수정됨] 내부 구조체 정의 삭제, 클래스 멤버 mTreeVertices 활용
    // 처음 생성 시 기본 나무 16개를 셋팅합니다.
    if (mTreeVertices.empty())
    {
        const int treeCount = 16;
        for (UINT i = 0; i < treeCount; ++i)
        {
            TreeSpriteVertex v;
            v.Pos.x = MathHelper::RandF(-45.0f, 45.0f);
            v.Pos.z = MathHelper::RandF(-45.0f, 45.0f);
            v.Pos.y = Hills::GetHeight(v.Pos.x, v.Pos.z) + 8.0f;
            v.Size = DirectX::XMFLOAT2(20.0f, 20.0f);
            mTreeVertices.push_back(v);
        }
    }

    std::vector<std::uint16_t> indices(mTreeVertices.size());
    for (size_t i = 0; i < mTreeVertices.size(); ++i)
        indices[i] = (std::uint16_t)i;

    const UINT vbByteSize = (UINT)mTreeVertices.size() * sizeof(TreeSpriteVertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    BuildGeometry(cmdList, "treeSpritesGeo", "points", mTreeVertices.data(), sizeof(TreeSpriteVertex), vbByteSize, indices);
}

// [추가됨] 새 나무 데이터를 배열에 추가하는 함수
void ResourceManager::AddTree(float x, float y, float z)
{
    TreeSpriteVertex v;
    v.Pos = DirectX::XMFLOAT3(x, y + 8.0f, z); // 살짝 띄움
    v.Size = DirectX::XMFLOAT2(20.0f, 20.0f);
    mTreeVertices.push_back(v);
}


// [추가됨] GPU 버퍼를 갱신하는 캡슐화된 로직
void ResourceManager::UpdateTreeGeometryBuffer(ID3D12GraphicsCommandList* cmdList)
{
    if (mTreeVertices.empty()) return;

    MeshGeometry* treeGeo = mGeometries["treeSpritesGeo"].get();
    if (!treeGeo) return;

    UINT vbByteSize = (UINT)mTreeVertices.size() * sizeof(TreeSpriteVertex);

    std::vector<std::uint16_t> indices(mTreeVertices.size());
    for (size_t i = 0; i < mTreeVertices.size(); ++i)
        indices[i] = (std::uint16_t)i;
    UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &treeGeo->VertexBufferCPU));
    CopyMemory(treeGeo->VertexBufferCPU->GetBufferPointer(), mTreeVertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &treeGeo->IndexBufferCPU));
    CopyMemory(treeGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    treeGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, mTreeVertices.data(), vbByteSize, treeGeo->VertexBufferUploader);

    treeGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, indices.data(), ibByteSize, treeGeo->IndexBufferUploader);

    treeGeo->VertexByteStride = sizeof(TreeSpriteVertex);
    treeGeo->VertexBufferByteSize = vbByteSize;
    treeGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    treeGeo->IndexBufferByteSize = ibByteSize;

    treeGeo->DrawArgs["points"].IndexCount = (UINT)mTreeVertices.size();
    treeGeo->DrawArgs["points"].StartIndexLocation = 0;
    treeGeo->DrawArgs["points"].BaseVertexLocation = 0;
}


void ResourceManager::BuildMaterials()
{
    BuildMaterial("grass", 0, 0, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f), 0.125f);
    BuildMaterial("water", 1, 1, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f), 0.0f);
    BuildMaterial("wirefence", 2, 2, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f), 0.25f);
    BuildMaterial("treeSprites", 3, 3, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f), 0.125f);
}

void ResourceManager::BuildMaterial(std::string name, int matCBIndex, int diffuseSrvHeapIndex, DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness)
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

float Hills::GetHeight(float x, float z)
{
    return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

DirectX::XMFLOAT3 Hills::GetNormal(float x, float z)
{
    DirectX::XMFLOAT3 n(
        -0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
        1.0f,
        -0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));
    DirectX::XMVECTOR unitNormal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&n));
    DirectX::XMStoreFloat3(&n, unitNormal);
    return n;
}