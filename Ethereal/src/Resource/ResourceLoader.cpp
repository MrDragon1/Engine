#include "ResourceLoader.h"

#include <Base/GlobalContext.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>

#include "Backend/Driver.h"
#include "stb_image.h"
namespace Ethereal {
namespace Utils {
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

Quaternion QuatFromAssimpQuat(const aiQuaternion& pOrientation) { return Quaternion(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z); }

}  // namespace Utils

static const uint32_t sMeshImportFlags = aiProcess_CalcTangentSpace |  // Create binormals/tangents just in case
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
    virtual void write(const char* message) override {
        std::string msg = message;
        // remove the end /n
        if (msg.ends_with('\n')) msg.pop_back();
        ET_CORE_ERROR("Assimp error: {0}", msg.c_str());
    }
};

Ref<Texture> ResourceLoader::LoadTexture(const std::string& path, bool flip) {
    Ref<Texture> texture;
    stbi_set_flip_vertically_on_load(flip);
    int width, height, channels;
    bool hdr = path.ends_with(".hdr");
    void* data;
    if (hdr)
        data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
    else
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (data) {
        auto api = GlobalContext::GetDriverApi();

        size_t datasize = width * height * channels;
        if (hdr) datasize *= sizeof(float);

        uint8_t* pixels = (uint8_t*)calloc(datasize, 1);
        memcpy(pixels, data, datasize);

        PixelDataFormat dataFormat;
        TextureFormat textureFormat;
        if (hdr) {
            if (channels == 4) {
                textureFormat = TextureFormat::R16G16B16A16_HDR;
                dataFormat = PixelDataFormat::RGBA;
            } else if (channels == 3) {
                textureFormat = TextureFormat::R16G16B16_HDR;
                dataFormat = PixelDataFormat::RGB;
            }

        } else {
            if (channels == 4) {
                textureFormat = TextureFormat::R8G8B8A8_UNORM;
                dataFormat = PixelDataFormat::RGBA;
            } else if (channels == 3) {
                textureFormat = TextureFormat::R8G8B8_UNORM;
                dataFormat = PixelDataFormat::RGB;
            } else if (channels == 2) {
                textureFormat = TextureFormat::R32G32_FLOAT;
                dataFormat = PixelDataFormat::RG;
            } else if (channels == 1) {
                textureFormat = TextureFormat::R8;
                dataFormat = PixelDataFormat::R;
            }
        }

        texture = api->CreateTexture(1, width, height, 1, textureFormat, TextureUsage::DEFAULT, TextureType::TEXTURE_2D);
        PixelBufferDescriptor pixelDesc(pixels, datasize, dataFormat,
                                        hdr ? PixelDataType::FLOAT : PixelDataType::UBYTE);  // The image loaded is 4 channel
        api->SetTextureData(texture, 0, 0, 0, 0, width, height, 1, std::move(pixelDesc));

        stbi_image_free(data);
    } else {
        ET_CORE_WARN("Failed to load texture: {0}", path);
        texture = nullptr;
    }
    return texture;
}

Ref<MeshSource> ResourceLoader::LoadMeshSource(const std::string& path) {
    Ref<MeshSource> ms = Ref<MeshSource>::Create();

    LogStream::Initialize();

    ET_CORE_INFO("Loading mesh: {0}", path.c_str());

    ms->mImporter = std::make_unique<Assimp::Importer>();
    ms->mFilePath = path;
    const aiScene* scene = ms->mImporter->ReadFile(path, sMeshImportFlags);
    if (!scene || !scene->HasMeshes()) {
        ET_CORE_ERROR("Failed to load mesh file: {0}", path.c_str());
        ms->SetFlag(AssetFlag::Invalid);
        return ms;
    }
    ms->mScene = scene;
    ms->mInverseTransform = Math::Inverse(Utils::Mat4FromAssimpMat4(scene->mRootNode->mTransformation));
    ms->mIsAnimated = scene->mAnimations != nullptr;
    if (ms->mIsAnimated) {
        Ref<Skeleton> skel = Ref<Skeleton>::Create();
        Ref<Animation> anim = Ref<Animation>::Create();
        ms->mAnimator = Ref<Animator>::Create(anim, skel);
    }

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    ms->mBoundingBox.Min = {FLT_MAX, FLT_MAX, FLT_MAX};
    ms->mBoundingBox.Max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    ms->mSubmeshes.reserve(scene->mNumMeshes);

    for (unsigned m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];
        Submesh& submesh = ms->mSubmeshes.emplace_back();
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
        if (ms->mIsAnimated) {
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

                ms->mAnimationVertices.push_back(vertex);
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

                ms->mStaticVertices.push_back(vertex);
            }
        }

        // Indices
        for (size_t i = 0; i < mesh->mNumFaces; i++) {
            ET_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
            Index index = {mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]};
            ms->mIndices.push_back(index);
        }
    }

    TraverseNodes(ms, scene->mRootNode);

    for (const auto& submesh : ms->mSubmeshes) {
        AABB transformedSubmeshAABB = submesh.BoundingBox;
        Vector3 min = Vector3(submesh.Transform * Vector4(transformedSubmeshAABB.Min, 1.0f));
        Vector3 max = Vector3(submesh.Transform * Vector4(transformedSubmeshAABB.Max, 1.0f));

        ms->mBoundingBox.Min.x = Math::Min(ms->mBoundingBox.Min.x, min.x);
        ms->mBoundingBox.Min.y = Math::Min(ms->mBoundingBox.Min.y, min.y);
        ms->mBoundingBox.Min.z = Math::Min(ms->mBoundingBox.Min.z, min.z);
        ms->mBoundingBox.Max.x = Math::Max(ms->mBoundingBox.Max.x, max.x);
        ms->mBoundingBox.Max.y = Math::Max(ms->mBoundingBox.Max.y, max.y);
        ms->mBoundingBox.Max.z = Math::Max(ms->mBoundingBox.Max.z, max.z);
    }

    // Bones
    if (ms->mIsAnimated) {
        // Create Hierarchy Joint Skeleton without offset matrix
        TraverseNodesAnim(ms, scene->mRootNode, ms->mAnimator->mSkeleton->mRoot);

        // Update offset matrix
        for (size_t m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];
            Submesh& submesh = ms->mSubmeshes[m];

            for (size_t i = 0; i < mesh->mNumBones; i++) {
                aiBone* bone = mesh->mBones[i];
                std::string boneName(bone->mName.data);
                size_t boneID = ms->mAnimator->mSkeleton->mNameIDMap[boneName];
                ET_CORE_ASSERT(boneID >= 0, "Can not find bone with name {0}", boneName);
                Ref<Joint> joint = ms->mAnimator->mSkeleton->mJointsMap[boneID];

                joint->mOffsetMatrix = Utils::Mat4FromAssimpMat4(bone->mOffsetMatrix);
                joint->mInverseOffsetMatrix = Math::Inverse(joint->mOffsetMatrix);

                for (size_t j = 0; j < bone->mNumWeights; j++) {
                    int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
                    float Weight = bone->mWeights[j].mWeight;
                    ms->mAnimationVertices[VertexID].AddBoneData(boneID, Weight);
                }
            }
        }

        // Create Animation (Only support one anim for now)
        auto& animation = ms->mAnimator->mAnimation;
        auto& rawAnim = scene->mAnimations[0];
        animation->mName = rawAnim->mName.data;
        animation->mDuration = rawAnim->mDuration;
        animation->mFramesPersecond = rawAnim->mTicksPerSecond;
        animation->mKeyClips.clear();

        // fill key clip
        auto& channels = rawAnim->mChannels;
        auto& keyClips = animation->mKeyClips;
        for (int i = 0; i < rawAnim->mNumChannels; i++) {
            aiNodeAnim* channel = channels[i];
            AnimKeyClip keyClip;
            keyClip.JointID = ms->mAnimator->mSkeleton->GetJointID(channel->mNodeName.data);

            int mNumPositions = channel->mNumPositionKeys;
            for (int positionIndex = 0; positionIndex < mNumPositions; ++positionIndex) {
                aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
                float timeStamp = channel->mPositionKeys[positionIndex].mTime;
                AnimPositionState data;
                data.Position = Utils::Vec3FromAssimpVec3(aiPosition);
                data.TimeStamp = timeStamp;
                keyClip.PositionStates.push_back(data);
            }

            int mNumRotations = channel->mNumRotationKeys;
            for (int rotationIndex = 0; rotationIndex < mNumRotations; ++rotationIndex) {
                aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
                float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
                AnimRotationState data;
                data.Rotation = Utils::QuatFromAssimpQuat(aiOrientation);
                data.TimeStamp = timeStamp;
                keyClip.RotationStates.push_back(data);
            }

            int mNumScalings = channel->mNumScalingKeys;
            for (int keyIndex = 0; keyIndex < mNumScalings; ++keyIndex) {
                aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
                float timeStamp = channel->mScalingKeys[keyIndex].mTime;
                AnimScaleState data;
                data.Scale = Utils::Vec3FromAssimpVec3(scale);
                data.TimeStamp = timeStamp;
                keyClip.ScaleStates.push_back(data);
            }

            animation->mKeyClips.push_back(keyClip);
        }
    }

    {
        auto api = GlobalContext::GetDriverApi();
        VertexBufferHandle vertexBuffer;
        if (ms->mIsAnimated) {
            AttributeArray attributes = {Attribute{
                                             .offset = 0,
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::FLOAT3,
                                             .flags = 0,
                                         },
                                         Attribute{
                                             .offset = sizeof(Vector3),
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::FLOAT3,
                                             .flags = 0,
                                         },
                                         Attribute{
                                             .offset = 2 * sizeof(Vector3),
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::FLOAT3,
                                             .flags = 0,
                                         },
                                         Attribute{
                                             .offset = 3 * sizeof(Vector3),
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::FLOAT3,
                                             .flags = 0,
                                         },
                                         Attribute{
                                             .offset = 4 * sizeof(Vector3),
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::FLOAT2,
                                             .flags = 0,
                                         },
                                         Attribute{
                                             .offset = 14 * sizeof(float),
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::INT4,
                                             .flags = Attribute::FLAG_INTEGER_TARGET,
                                         },
                                         Attribute{
                                             .offset = 14 * sizeof(float) + 4 * sizeof(int),
                                             .stride = 18 * sizeof(float) + 4 * sizeof(int),
                                             .buffer = 0,
                                             .type = ElementType::FLOAT4,
                                             .flags = 0,
                                         }

            };

            const size_t size = sizeof(AnimationVertex) * ms->mAnimationVertices.size();
            BufferObjectHandle bufferObject = api->CreateBufferObject(size, BufferObjectBinding::VERTEX, BufferUsage::STATIC);
            BufferDescriptor vertexBufferDesc(ms->mAnimationVertices.data(), size);
            api->UpdateBufferObject(bufferObject, std::move(vertexBufferDesc), 0);

            vertexBuffer = api->CreateVertexBuffer(attributes, ms->mAnimationVertices.size(), 7, 1);
            api->SetVertexBufferObject(vertexBuffer, 0, bufferObject);
        } else {
            AttributeArray attributes = {
                Attribute{
                    .offset = 0,
                    .stride = 14 * sizeof(float),
                    .buffer = 0,
                    .type = ElementType::FLOAT3,
                    .flags = 0,
                },
                Attribute{
                    .offset = sizeof(Vector3),
                    .stride = 14 * sizeof(float),
                    .buffer = 0,
                    .type = ElementType::FLOAT3,
                    .flags = 0,
                },
                Attribute{
                    .offset = 2 * sizeof(Vector3),
                    .stride = 14 * sizeof(float),
                    .buffer = 0,
                    .type = ElementType::FLOAT3,
                    .flags = 0,
                },
                Attribute{
                    .offset = 3 * sizeof(Vector3),
                    .stride = 14 * sizeof(float),
                    .buffer = 0,
                    .type = ElementType::FLOAT3,
                    .flags = 0,
                },
                Attribute{
                    .offset = 4 * sizeof(Vector3),
                    .stride = 14 * sizeof(float),
                    .buffer = 0,
                    .type = ElementType::FLOAT2,
                    .flags = 0,
                },
            };

            const size_t size = sizeof(Vertex) * ms->mStaticVertices.size();
            BufferObjectHandle bufferObject = api->CreateBufferObject(size, BufferObjectBinding::VERTEX, BufferUsage::STATIC);
            BufferDescriptor vertexBufferDesc(ms->mStaticVertices.data(), size);
            api->UpdateBufferObject(bufferObject, std::move(vertexBufferDesc), 0);

            vertexBuffer = api->CreateVertexBuffer(attributes, ms->mStaticVertices.size(), 5, 1);
            api->SetVertexBufferObject(vertexBuffer, 0, bufferObject);
        }

        uint32_t mIndexCount = 3 * ms->mIndices.size();
        ElementType elementType = ElementType::UINT;
        static_assert(sizeof(uint32_t) == 4);
        IndexBufferHandle indexBuffer = api->CreateIndexBuffer(elementType, mIndexCount, BufferUsage::STATIC);
        BufferDescriptor indexBufferDesc(ms->mIndices.data(), mIndexCount * sizeof(uint32_t));
        api->UpdateIndexBuffer(indexBuffer, std::move(indexBufferDesc), 0);

        ms->mRenderPrimitive = api->CreateRenderPrimitive(vertexBuffer, indexBuffer, PrimitiveType::TRIANGLES, 0, 0, 0, mIndexCount);
    }
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
    ms->mNodeMap[node].resize(node->mNumMeshes);
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        uint32_t mesh = node->mMeshes[i];
        auto& submesh = ms->mSubmeshes[mesh];
        submesh.NodeName = node->mName.C_Str();
        submesh.Transform = transform;
        submesh.LocalTransform = localTransform;
        ms->mNodeMap[node][i] = mesh;
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++) TraverseNodes(ms, node->mChildren[i], transform, level + 1);
}

void ResourceLoader::TraverseNodesAnim(Ref<MeshSource>& ms, aiNode* node, Ref<Joint>& joint) {
    ET_CORE_ASSERT(node, "Empty aiNode!")
    joint->mName = node->mName.data;
    joint->mID = ms->mJointCount++;
    ms->mAnimator->mSkeleton->mNameIDMap[joint->mName] = joint->mID;
    ms->mAnimator->mSkeleton->mJointsMap[joint->mID] = joint;
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        Ref<Joint> childJoint = Ref<Joint>::Create();
        childJoint->mParent = joint;
        TraverseNodesAnim(ms, node->mChildren[i], childJoint);
        joint->mChildren.push_back(childJoint);
    }
}

}  // namespace Ethereal