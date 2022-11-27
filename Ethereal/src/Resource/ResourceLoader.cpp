#include "ResourceLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <Base/GlobalContext.h>

namespace Ethereal
{
    namespace Utils
    {
        Matrix4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix) {
            Matrix4 result;
            // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            result[0][0] = matrix.a1;
            result[1][0] = matrix.a2;
            result[2][0] = matrix.a3;
            result[3][0] = matrix.a4;
            result[0][1] = matrix.b1;
            result[1][1] = matrix.b2;
            result[2][1] = matrix.b3;
            result[3][1] = matrix.b4;
            result[0][2] = matrix.c1;
            result[1][2] = matrix.c2;
            result[2][2] = matrix.c3;
            result[3][2] = matrix.c4;
            result[0][3] = matrix.d1;
            result[1][3] = matrix.d2;
            result[2][3] = matrix.d3;
            result[3][3] = matrix.d4;
            return result;
        }

        Vector3 Vec3FromAssimpVec3(const aiVector3D& vec) { return Vector3(vec.x, vec.y, vec.z); }

        Quaternion QuatFromAssimpQuat(const aiQuaternion& pOrientation) {
            return Quaternion(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }

    }  // namespace Utils

    static const uint32_t s_MeshImportFlags = aiProcess_CalcTangentSpace |  // Create binormals/tangents just in case
                                              aiProcess_Triangulate |       // Make sure we're triangles
                                              aiProcess_SortByPType |       // Split meshes by primitive type
                                              aiProcess_GenNormals |        // Make sure we have legit normals
                                              aiProcess_GenUVCoords |       // Convert UVs if required
                                                                            // 		aiProcess_OptimizeGraph |
                                              aiProcess_OptimizeMeshes |    // Batch draws where possible
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_GlobalScale |  // e.g. convert cm to m for fbx import (and other formats where cm is native)
                                              aiProcess_ValidateDataStructure;  // Validation

    struct LogStream : public Assimp::LogStream {
        static void Initialize() {
            if (Assimp::DefaultLogger::isNullLogger()) {
                Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
                Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
            }
        }
        virtual void write(const char* message) override { ET_CORE_ERROR("Assimp error: {0}", message); }
    };

    Ref<Texture> ResourceLoader::LoadTexture(const std::string& path) {
        return Texture2D::Create(path);
    }

    Ref<MeshSource> ResourceLoader::LoadMeshSource(const std::string& path) {
        Ref<MeshSource> ms = Ref<MeshSource>::Create();

        LogStream::Initialize();

        ET_CORE_INFO("Loading mesh: {0}", path.c_str());

        ms->m_Importer = std::make_unique<Assimp::Importer>();
        ms->m_FilePath = path;
        const aiScene* scene = ms->m_Importer->ReadFile(path, s_MeshImportFlags);
        if (!scene || !scene->HasMeshes()) {
            ET_CORE_ERROR("Failed to load mesh file: {0}", path.c_str());
            ms->SetFlag(AssetFlag::Invalid);
            return ms;
        }
        ms->m_Scene = scene;
        ms->m_InverseTransform = Math::Inverse(Utils::Mat4FromAssimpMat4(scene->mRootNode->mTransformation));
        ms->m_IsAnimated = scene->mAnimations != nullptr;
        if (ms->m_IsAnimated) {
            Ref<Skeleton> skel = Ref<Skeleton>::Create();
            Ref<Animation> anim = Ref<Animation>::Create();
            ms->m_Animator = Ref<Animator>::Create(anim, skel);
        }

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        ms->m_BoundingBox.Min = {FLT_MAX, FLT_MAX, FLT_MAX};
        ms->m_BoundingBox.Max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        ms->m_Submeshes.reserve(scene->mNumMeshes);

        for (unsigned m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];
            Submesh& submesh = ms->m_Submeshes.emplace_back();
            submesh.BaseVertex = vertexCount;
            submesh.BaseIndex = indexCount;
            submesh.MaterialIndex = mesh->mMaterialIndex;
            submesh.VertexCount = mesh->mNumVertices;
            submesh.IndexCount = mesh->mNumFaces * 3;
            submesh.MeshName = mesh->mName.C_Str();
            vertexCount += mesh->mNumVertices;
            indexCount += submesh.IndexCount;

            ET_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
            ET_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

            // Vertices
            if (ms->m_IsAnimated) {
                for (size_t i = 0; i < mesh->mNumVertices; i++) {
                    AnimationVertex vertex;
                    vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
                    vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

                    if (mesh->HasTangentsAndBitangents()) {
                        vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                        vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                    }

                    if (mesh->HasTextureCoords(0))
                        vertex.Texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                    else
                        vertex.Texcoord = {0, 0};

                    ms->m_AnimationVertices.push_back(vertex);
                }
            } else {
                auto& aabb = submesh.BoundingBox;
                aabb.Min = {FLT_MAX, FLT_MAX, FLT_MAX};
                aabb.Max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
                for (size_t i = 0; i < mesh->mNumVertices; i++) {
                    Vertex vertex;
                    vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
                    vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
                    aabb.Min.x = Math::Min(vertex.Position.x, aabb.Min.x);
                    aabb.Min.y = Math::Min(vertex.Position.y, aabb.Min.y);
                    aabb.Min.z = Math::Min(vertex.Position.z, aabb.Min.z);
                    aabb.Max.x = Math::Max(vertex.Position.x, aabb.Max.x);
                    aabb.Max.y = Math::Max(vertex.Position.y, aabb.Max.y);
                    aabb.Max.z = Math::Max(vertex.Position.z, aabb.Max.z);

                    if (mesh->HasTangentsAndBitangents()) {
                        vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                        vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                    }

                    if (mesh->HasTextureCoords(0))
                        vertex.Texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                    else
                        vertex.Texcoord = {0, 0};

                    ms->m_StaticVertices.push_back(vertex);
                }
            }

            // Indices
            for (size_t i = 0; i < mesh->mNumFaces; i++) {
                ET_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
                Index index = {mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]};
                ms->m_Indices.push_back(index);
            }
        }

        TraverseNodes(ms, scene->mRootNode);

        for (const auto& submesh : ms->m_Submeshes) {
            AABB transformedSubmeshAABB = submesh.BoundingBox;
            Vector3 min = Vector3(submesh.Transform * Vector4(transformedSubmeshAABB.Min, 1.0f));
            Vector3 max = Vector3(submesh.Transform * Vector4(transformedSubmeshAABB.Max, 1.0f));

            ms->m_BoundingBox.Min.x = Math::Min(ms->m_BoundingBox.Min.x, min.x);
            ms->m_BoundingBox.Min.y = Math::Min(ms->m_BoundingBox.Min.y, min.y);
            ms->m_BoundingBox.Min.z = Math::Min(ms->m_BoundingBox.Min.z, min.z);
            ms->m_BoundingBox.Max.x = Math::Max(ms->m_BoundingBox.Max.x, max.x);
            ms->m_BoundingBox.Max.y = Math::Max(ms->m_BoundingBox.Max.y, max.y);
            ms->m_BoundingBox.Max.z = Math::Max(ms->m_BoundingBox.Max.z, max.z);
        }

        // Bones
        if (ms->m_IsAnimated) {
            // Create Hierarchy Joint Skeleton without offset matrix
            TraverseNodesAnim(ms, scene->mRootNode, ms->m_Animator->m_Skeleton->m_Root);

            // Update offset matrix
            for (size_t m = 0; m < scene->mNumMeshes; m++) {
                aiMesh* mesh = scene->mMeshes[m];
                Submesh& submesh = ms->m_Submeshes[m];

                for (size_t i = 0; i < mesh->mNumBones; i++) {
                    aiBone* bone = mesh->mBones[i];
                    std::string boneName(bone->mName.data);
                    size_t boneID = ms->m_Animator->m_Skeleton->m_NameIDMap[boneName];
                    ET_CORE_ASSERT(boneID >= 0, "Can not find bone with name {0}", boneName);
                    Ref<Joint> joint = ms->m_Animator->m_Skeleton->m_JointsMap[boneID];

                    joint->m_OffsetMatrix = Utils::Mat4FromAssimpMat4(bone->mOffsetMatrix);
                    joint->m_InverseOffsetMatrix = Math::Inverse(joint->m_OffsetMatrix);

                    for (size_t j = 0; j < bone->mNumWeights; j++) {
                        int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
                        float Weight = bone->mWeights[j].mWeight;
                        ms->m_AnimationVertices[VertexID].AddBoneData(boneID, Weight);
                    }
                }
            }

            // Create Animation (Only support one anim for now)
            auto& animation = ms->m_Animator->m_Animation;
            auto& rawAnim = scene->mAnimations[0];
            animation->m_Name = rawAnim->mName.data;
            animation->m_Duration = rawAnim->mDuration;
            animation->m_FramesPersecond = rawAnim->mTicksPerSecond;
            animation->m_KeyClips.clear();

            // fill key clip
            auto& channels = rawAnim->mChannels;
            auto& keyClips = animation->m_KeyClips;
            for (int i = 0; i < rawAnim->mNumChannels; i++) {
                aiNodeAnim* channel = channels[i];
                AnimKeyClip keyClip;
                keyClip.JointID = ms->m_Animator->m_Skeleton->GetJointID(channel->mNodeName.data);

                int m_NumPositions = channel->mNumPositionKeys;
                for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
                    aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
                    float timeStamp = channel->mPositionKeys[positionIndex].mTime;
                    AnimPositionState data;
                    data.Position = Utils::Vec3FromAssimpVec3(aiPosition);
                    data.TimeStamp = timeStamp;
                    keyClip.PositionStates.push_back(data);
                }

                int m_NumRotations = channel->mNumRotationKeys;
                for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
                    aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
                    float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
                    AnimRotationState data;
                    data.Rotation = Utils::QuatFromAssimpQuat(aiOrientation);
                    data.TimeStamp = timeStamp;
                    keyClip.RotationStates.push_back(data);
                }

                int m_NumScalings = channel->mNumScalingKeys;
                for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
                    aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
                    float timeStamp = channel->mScalingKeys[keyIndex].mTime;
                    AnimScaleState data;
                    data.Scale = Utils::Vec3FromAssimpVec3(scale);
                    data.TimeStamp = timeStamp;
                    keyClip.ScaleStates.push_back(data);
                }

                animation->m_KeyClips.push_back(keyClip);
            }
        }

        if (ms->m_IsAnimated) {
            ms->m_VertexBuffer = VertexBuffer::Create(ms->m_AnimationVertices.data(), (uint32_t)(ms->m_AnimationVertices.size() * sizeof(AnimationVertex)));
            ms->m_VertexBufferLayout = {
                {ShaderDataType::Float3, "a_Position"},    {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
                {ShaderDataType::Float3, "a_Binormal"},    {ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int4, "a_BoneIDs"},
                {ShaderDataType::Float4, "a_BoneWeights"},
            };
        } else {
            ms->m_VertexBuffer = VertexBuffer::Create(ms->m_StaticVertices.data(), (uint32_t)(ms->m_StaticVertices.size() * sizeof(Vertex)));
            ms->m_VertexBufferLayout = {
                {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
                {ShaderDataType::Float3, "a_Binormal"}, {ShaderDataType::Float2, "a_TexCoord"},
            };
        }

        ms->m_IndexBuffer = IndexBuffer::Create(ms->m_Indices.data(), (uint32_t)(ms->m_Indices.size() * sizeof(Index)));

        ms->m_VertexArray = VertexArray::Create();
        ms->m_VertexBuffer->SetLayout(ms->m_VertexBufferLayout);
        ms->m_VertexArray->AddVertexBuffer(ms->m_VertexBuffer);
        ms->m_VertexArray->SetIndexBuffer(ms->m_IndexBuffer);
        return ms;
    }

    Ref<Environment> ResourceLoader::LoadEnvironmentMap(const std::string& path) {
        auto [radiance, irradiance] = GlobalContext::GetRenderSystem().CreateEnvironmentMap(path);

        if (!radiance || !irradiance) return nullptr;

        auto asset = Ref<Environment>::Create(radiance, irradiance);
        return asset;
    }


    void ResourceLoader::TraverseNodes(Ref<MeshSource>& ms, aiNode* node, const Matrix4& parentTransform, uint32_t level) {
        Matrix4 localTransform = Utils::Mat4FromAssimpMat4(node->mTransformation);
        Matrix4 transform = parentTransform * localTransform;
        ms->m_NodeMap[node].resize(node->mNumMeshes);
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            uint32_t mesh = node->mMeshes[i];
            auto& submesh = ms->m_Submeshes[mesh];
            submesh.NodeName = node->mName.C_Str();
            submesh.Transform = transform;
            submesh.LocalTransform = localTransform;
            ms->m_NodeMap[node][i] = mesh;
        }
        for (uint32_t i = 0; i < node->mNumChildren; i++) TraverseNodes(ms, node->mChildren[i], transform, level + 1);
    }

    void ResourceLoader::TraverseNodesAnim(Ref<MeshSource>& ms, aiNode* node, Ref<Joint>& joint) {
        ET_CORE_ASSERT(node, "Empty aiNode!")
        joint->m_Name = node->mName.data;
        joint->m_ID = ms->m_JointCount++;
        ms->m_Animator->m_Skeleton->m_NameIDMap[joint->m_Name] = joint->m_ID;
        ms->m_Animator->m_Skeleton->m_JointsMap[joint->m_ID] = joint;
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            Ref<Joint> childJoint = Ref<Joint>::Create();
            childJoint->m_Parent = joint;
            TraverseNodesAnim(ms, node->mChildren[i], childJoint);
            joint->m_Children.push_back(childJoint);
        }
    }

}  // namespace Ethereal