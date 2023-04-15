#pragma once
#include <Backend/Driver.h>

#include <vector>

#include "Base/AABB.h"
#include "Base/Math/Vector.h"
#include "Base/TimeStamp.h"
#include "Core/Animation/Animator.h"
#include "Core/Asset/Asset.h"
#include "Core/Renderer/MaterialAsset.h"
#include "Resource/Descriptor/Mesh.h"

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp {
class Importer;
}
namespace Ethereal {
struct Vertex {
    Vector3 Position;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 Binormal;
    Vector2 Texcoord;
};

struct AnimationVertex {
    Vector3 Position;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 Binormal;
    Vector2 Texcoord;

    uint32_t IDs[4] = {0, 0, 0, 0};
    float Weights[4]{0.0f, 0.0f, 0.0f, 0.0f};

    void AddBoneData(uint32_t BoneID, float Weight) {
        for (size_t i = 0; i < 4; i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        // TODO: Keep top weights
        ET_CORE_WARN("Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})", BoneID, Weight);
    }
};

struct Index {
    uint32_t V1, V2, V3;
};

static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

struct Triangle {
    Vertex V0, V1, V2;

    Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) : V0(v0), V1(v1), V2(v2) {}
};

class Submesh {
   public:
    uint32_t BaseVertex;
    uint32_t BaseIndex;
    uint32_t MaterialIndex;
    uint32_t IndexCount;
    uint32_t VertexCount;

    Matrix4 Transform{Matrix4::IDENTITY};  // World transform
    Matrix4 LocalTransform{Matrix4::IDENTITY};
    AABB BoundingBox;

    std::string NodeName, MeshName;
};

// MeshSource is a representation of an actual asset file on disk
// Meshes are created from MeshSource
class MeshSource : public Asset {
   public:
    MeshSource();
    virtual ~MeshSource();

    std::vector<Submesh>& GetSubmeshes() { return mSubmeshes; }
    const std::vector<Submesh>& GetSubmeshes() const { return mSubmeshes; }

    const std::vector<Vertex>& GetVertices() const { return mStaticVertices; }
    const std::vector<AnimationVertex>& GetAnimationVertices() const { return mAnimationVertices; }
    const std::vector<Index>& GetIndices() const { return mIndices; }

    const std::string& GetFilePath() const { return mFilePath; }

    RenderPrimitiveHandle GetRenderPrimitive() { return mRenderPrimitive; }
    RenderPrimitiveHandle GetSubMeshRenderPrimitive(uint32_t index);

    Ref<Animator> GetAnimator() { return mAnimator; }

    static AssetType GetStaticType() { return AssetType::MeshSource; }
    virtual AssetType GetAssetType() const override { return GetStaticType(); }

    const AABB& GetBoundingBox() const { return mBoundingBox; }
    bool IsAnimated() const { return mIsAnimated; }
    void LoadMaterials(Ref<MaterialTable>& materials);

   private:
    void TraverseNodes(aiNode* node, const Matrix4& parentTransform = Matrix4::IDENTITY, uint32_t level = 0);
    void TraverseNodesAnim(aiNode* node, Ref<Joint>& joint);

   private:
    std::unique_ptr<Assimp::Importer> mImporter;
    const aiScene* mScene;

    std::string mFilePath;

    Matrix4 mInverseTransform;

    RenderPrimitiveHandle mRenderPrimitive;

    std::vector<Submesh> mSubmeshes;
    AABB mBoundingBox;

    std::vector<Vertex> mStaticVertices;
    std::vector<Index> mIndices;
    std::unordered_map<aiNode*, std::vector<uint32_t>> mNodeMap;

    bool mIsAnimated;
    std::vector<AnimationVertex> mAnimationVertices;
    Ref<Animator> mAnimator;
    size_t mJointCount = 0;

    friend class ResourceLoader;
};

// Static Mesh - no skeletal animation, flattened hierarchy
class StaticMesh : public Asset {
   public:
    explicit StaticMesh(Ref<MeshSource> meshSource, Ref<MaterialTable> materialTable);
    StaticMesh(Ref<MeshSource> meshSource, const std::vector<uint32_t>& submeshes);
    StaticMesh(const Ref<StaticMesh>& other);
    StaticMesh(){};
    StaticMesh(const StaticMeshDesc& desc) { Load(desc); }
    virtual ~StaticMesh();

    void Load(const StaticMeshDesc& desc);
    void Save(StaticMeshDesc& desc);

    std::vector<uint32_t>& GetSubmeshes() { return mSubmeshes; }
    const std::vector<uint32_t>& GetSubmeshes() const { return mSubmeshes; }

    // Pass in an empty vector to set ALL submeshes for MeshSource
    void SetSubmeshes(const std::vector<uint32_t>& submeshes);

    Ref<MeshSource> GetMeshSource() { return mMeshSource; }
    Ref<MeshSource> GetMeshSource() const { return mMeshSource; }
    void SetMeshAsset(Ref<MeshSource> meshAsset) { mMeshSource = meshAsset; }

    Ref<MaterialTable> GetMaterials() const { return mMaterials; }

    static AssetType GetStaticType() { return AssetType::StaticMesh; }
    virtual AssetType GetAssetType() const override { return GetStaticType(); }

   private:
    Ref<MeshSource> mMeshSource;
    std::vector<uint32_t> mSubmeshes;

    // Materials
    Ref<MaterialTable> mMaterials;

    friend class Scene;
    friend class Renderer;
    friend class SceneHierarchyPanel;
};

class Mesh : public Asset {
   public:
    explicit Mesh(Ref<MeshSource> meshSource, Ref<MaterialTable> materialTable);
    Mesh(Ref<MeshSource> meshSource, const std::vector<uint32_t>& submeshes);
    Mesh(const Ref<Mesh>& other);
    Mesh(){};
    Mesh(const MeshDesc& desc) { Load(desc); }
    virtual ~Mesh();

    void Load(const MeshDesc& desc);
    void Save(MeshDesc& desc);

    std::vector<uint32_t>& GetSubmeshes() { return mSubmeshes; }
    const std::vector<uint32_t>& GetSubmeshes() const { return mSubmeshes; }

    // Pass in an empty vector to set ALL submeshes for MeshSource
    void SetSubmeshes(const std::vector<uint32_t>& submeshes);

    Ref<MeshSource> GetMeshSource() { return mMeshSource; }
    Ref<MeshSource> GetMeshSource() const { return mMeshSource; }
    void SetMeshAsset(Ref<MeshSource> meshAsset) { mMeshSource = meshAsset; }

    Ref<MaterialTable> GetMaterials() const { return mMaterials; }
    Ref<Animator> GetAnimator() { return mMeshSource->GetAnimator(); }

    static AssetType GetStaticType() { return AssetType::Mesh; }
    virtual AssetType GetAssetType() const override { return GetStaticType(); }

   private:
    Ref<MeshSource> mMeshSource;
    std::vector<uint32_t> mSubmeshes;

    // Materials
    Ref<MaterialTable> mMaterials;

    // Animation
    bool mIsAnimated = false;
    float mAnimationTime = 0.0f;
    float mWorldTime = 0.0f;
    float mTimeMultiplier = 1.0f;
    bool mAnimationPlaying = true;

    friend class Scene;
    friend class Renderer;
    friend class SceneHierarchyPanel;
};

}  // namespace Ethereal