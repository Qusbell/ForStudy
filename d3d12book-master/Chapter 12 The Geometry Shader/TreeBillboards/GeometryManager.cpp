#include "GeometryManager.h"


namespace HillsCalculate
{
    float GetHillsHeight(float x, float z)
    {
        return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
    }

    DirectX::XMFLOAT3 GetHillsNormal(float x, float z)
    {
        // n = (-df/dx, 1, -df/dz)
        DirectX::XMFLOAT3 n(
            -0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
            1.0f,
            -0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

        DirectX::XMVECTOR unitNormal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&n));
        DirectX::XMStoreFloat3(&n, unitNormal);

        return n;
    }
}


// [리팩토링] 생성자에서 Device와 CommandList를 주입받아 내부 상태로 저장
GeometryManager::GeometryManager(ID3D12Device* device)
    : md3dDevice(device)
{
}

void GeometryManager::BuildAllGeometries(ID3D12GraphicsCommandList* cmdList, Waves* waves)
{
    // 내부 함수들을 호출하여 mGeometries 맵을 채움
    BuildLandGeometry(cmdList);
    BuildWavesGeometry(cmdList, waves);
    BuildBoxGeometry(cmdList);
    BuildTreeSpritesGeometry(cmdList);
}

// [리팩토링] 맵에 저장된 Geometry의 포인터 반환. 찾지 못하면 nullptr 반환.
MeshGeometry* GeometryManager::GetGeometry(const std::string& name) const
{
    auto it = mGeometries.find(name);
    if (it != mGeometries.end())
    {
        return it->second.get();
    }
    return nullptr;
}

std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> GeometryManager::GetGeometries()
{
    return mGeometries;
}

void GeometryManager::BuildLandGeometry(ID3D12GraphicsCommandList* cmdList)
{
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

    std::vector<Vertex> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        auto& p = grid.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Pos.y = HillsCalculate::GetHillsHeight(p.x, p.z);
        vertices[i].Normal = HillsCalculate::GetHillsNormal(p.x, p.z);
        vertices[i].TexC = grid.Vertices[i].TexC;
    }

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

    std::vector<std::uint16_t> indices = grid.GetIndices16();
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = "landGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(Vertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    geo->DrawArgs["grid"] = submesh;

    mGeometries[geo->Name] = std::move(geo);
}

void GeometryManager::BuildWavesGeometry(ID3D12GraphicsCommandList* cmdList, Waves* waves)
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
    UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = "waterGeo";

    geo->VertexBufferCPU = nullptr;
    geo->VertexBufferGPU = nullptr;

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(Vertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    geo->DrawArgs["water"] = submesh;

    mGeometries[geo->Name] = std::move(geo);
}

void GeometryManager::BuildBoxGeometry(ID3D12GraphicsCommandList* cmdList)
{
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(8.0f, 8.0f, 8.0f, 3);

    std::vector<Vertex> vertices(box.Vertices.size());
    for (size_t i = 0; i < box.Vertices.size(); ++i)
    {
        auto& p = box.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Normal = box.Vertices[i].Normal;
        vertices[i].TexC = box.Vertices[i].TexC;
    }

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

    std::vector<std::uint16_t> indices = box.GetIndices16();
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = "boxGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(Vertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    geo->DrawArgs["box"] = submesh;

    mGeometries[geo->Name] = std::move(geo);
}

void GeometryManager::BuildTreeSpritesGeometry(ID3D12GraphicsCommandList* cmdList)
{
    struct TreeSpriteVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT2 Size;
    };

    static const int treeCount = 16;
    std::array<TreeSpriteVertex, 16> vertices;
    for (UINT i = 0; i < treeCount; ++i)
    {
        float x = MathHelper::RandF(-45.0f, 45.0f);
        float z = MathHelper::RandF(-45.0f, 45.0f);
        float y = HillsCalculate::GetHillsHeight(x, z);

        y += 8.0f;

        vertices[i].Pos = XMFLOAT3(x, y, z);
        vertices[i].Size = XMFLOAT2(20.0f, 20.0f);
    }

    std::array<std::uint16_t, 16> indices =
    {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15
    };

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = "treeSpritesGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(TreeSpriteVertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    geo->DrawArgs["points"] = submesh;

    mGeometries[geo->Name] = std::move(geo);
}