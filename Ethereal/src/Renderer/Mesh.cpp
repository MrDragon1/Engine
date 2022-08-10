#include "pch.h"
#include "Mesh.h"
#include "Core/GlobalContext.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "imgui.h"
#include <filesystem>
#include <Asset/AssetManager.h>

#define MESH_DEBUG_LOG 0
#if MESH_DEBUG_LOG
#define ET_MESH_LOG(...) ET_CORE_TRACE(__VA_ARGS__)
#define ET_MESH_ERROR(...) ET_CORE_ERROR(__VA_ARGS__)
#else
#define ET_MESH_LOG(...)
#define ET_MESH_ERROR(...)
#endif

namespace Ethereal
{
    namespace Utils
    {
        glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix) {
            glm::mat4 result;
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

    }  // namespace Utils

    static const uint32_t s_MeshImportFlags = aiProcess_CalcTangentSpace |  // Create binormals/tangents just in case
                                              aiProcess_Triangulate |       // Make sure we're triangles
                                              aiProcess_SortByPType |       // Split meshes by primitive type
                                              aiProcess_GenNormals |        // Make sure we have legit normals
                                              aiProcess_GenUVCoords |       // Convert UVs if required
                                                                            //		aiProcess_OptimizeGraph |
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
        virtual void write(const char* message) override { ET_MESH_ERROR("Assimp error: {0}", message); }
    };

    MeshSource::MeshSource(const std::string& filename) : m_FilePath(filename) {
        LogStream::Initialize();

        ET_CORE_INFO("Loading mesh: {0}", filename.c_str());

        m_Importer = std::make_unique<Assimp::Importer>();

        const aiScene* scene = m_Importer->ReadFile(filename, s_MeshImportFlags);
        if (!scene || !scene->HasMeshes()) {
            ET_CORE_ERROR("Failed to load mesh file: {0}", filename);
            SetFlag(AssetFlag::Invalid);
            return;
        }

        m_Scene = scene;
        m_InverseTransform = glm::inverse(Utils::Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        m_BoundingBox.Min = {FLT_MAX, FLT_MAX, FLT_MAX};
        m_BoundingBox.Max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        m_Submeshes.reserve(scene->mNumMeshes);

        for (unsigned m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];

            Submesh& submesh = m_Submeshes.emplace_back();
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

            auto& aabb = submesh.BoundingBox;
            aabb.Min = {FLT_MAX, FLT_MAX, FLT_MAX};
            aabb.Max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
            for (size_t i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;
                vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
                vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
                aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
                aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
                aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
                aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
                aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
                aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

                if (mesh->HasTangentsAndBitangents()) {
                    vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                    vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                }

                if (mesh->HasTextureCoords(0))
                    vertex.Texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                else
                    vertex.Texcoord = {0, 0};

                m_StaticVertices.push_back(vertex);
            }

            // Indices
            for (size_t i = 0; i < mesh->mNumFaces; i++) {
                ET_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
                Index index = {mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]};
                m_Indices.push_back(index);
            }
        }

        TraverseNodes(scene->mRootNode);

        for (const auto& submesh : m_Submeshes) {
            AABB transformedSubmeshAABB = submesh.BoundingBox;
            glm::vec3 min = glm::vec3(submesh.Transform * glm::vec4(transformedSubmeshAABB.Min, 1.0f));
            glm::vec3 max = glm::vec3(submesh.Transform * glm::vec4(transformedSubmeshAABB.Max, 1.0f));

            m_BoundingBox.Min.x = glm::min(m_BoundingBox.Min.x, min.x);
            m_BoundingBox.Min.y = glm::min(m_BoundingBox.Min.y, min.y);
            m_BoundingBox.Min.z = glm::min(m_BoundingBox.Min.z, min.z);
            m_BoundingBox.Max.x = glm::max(m_BoundingBox.Max.x, max.x);
            m_BoundingBox.Max.y = glm::max(m_BoundingBox.Max.y, max.y);
            m_BoundingBox.Max.z = glm::max(m_BoundingBox.Max.z, max.z);
        }

        // Materials
        if (scene->HasMaterials()) {
            ET_MESH_LOG("---- Materials - {0} ----", filename);
            m_Materials.resize(scene->mNumMaterials);
            for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
                auto aiMaterial = scene->mMaterials[i];
                auto aiMaterialName = aiMaterial->GetName();
                ET_CORE_INFO("  {0} (Index = {1})", aiMaterialName.data, i);
                auto mi = Ref<MaterialAsset>::Create(aiMaterialName.data);
                m_Materials[i] = mi;

                aiString aiTexPath;
                uint32_t textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
                ET_MESH_LOG("    TextureCount = {0}", textureCount);

                glm::vec3 albedoColor(0.8f);
                float emission = 0.0f;
                aiColor3D aiColor, aiEmission;
                if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS) albedoColor = {aiColor.r, aiColor.g, aiColor.b};

                if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS) emission = aiEmission.r;

                mi->SetAlbedoColor(albedoColor);
                mi->SetEmission(emission);

                float shininess, metalness;
                if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS) shininess = 80.0f;  // Default value

                if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS) metalness = 0.0f;

                float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
                ET_MESH_LOG("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
                ET_MESH_LOG("    ROUGHNESS = {0}", roughness);
                ET_MESH_LOG("    METALNESS = {0}", metalness);
                bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
                mi->SetUseAlbedoMap(hasAlbedoMap);
                bool fallback = !hasAlbedoMap;
                if (hasAlbedoMap) {
                    // TODO: Temp - this should be handled by Filesystem
                    std::filesystem::path path = filename;
                    auto parentPath = path.parent_path();
                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();
                    ET_MESH_LOG("    Albedo map path = {0}", texturePath);
                    auto texture = AssetManager::GetAsset<Texture>(texturePath);
                    if (texture->IsLoaded()) {
                        mi->SetAlbedoMap(texture);
                        mi->SetAlbedoColor(glm::vec3(1.0f));
                    } else {
                        ET_CORE_ERROR("Could not load texture: {0}", texturePath);
                        fallback = true;
                    }
                }

                if (fallback) {
                    ET_MESH_LOG("    No albedo map");
                    mi->ClearAlbedoMap();
                }

                // Normal maps
                bool hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS;
                mi->SetUseNormalMap(hasNormalMap);
                fallback = !hasNormalMap;
                if (hasNormalMap) {
                    // TODO: Temp - this should be handled by Filesystem
                    std::filesystem::path path = filename;
                    auto parentPath = path.parent_path();
                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();
                    ET_MESH_LOG("    Normal map path = {0}", texturePath);
                    auto texture = AssetManager::GetAsset<Texture>(texturePath);
                    if (texture->IsLoaded()) {
                        mi->SetNormalMap(texture);
                    } else {
                        ET_CORE_ERROR("    Could not load texture: {0}", texturePath);
                        fallback = true;
                    }
                }

                if (fallback) {
                    ET_MESH_LOG("    No normal map");
                    mi->ClearNormalMap();
                    mi->SetUseNormalMap(false);
                }

                // Roughness map
                bool hasRoughnessMap = aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS;
                mi->SetUseRoughnessMap(hasRoughnessMap);
                fallback = !hasRoughnessMap;
                if (hasRoughnessMap) {
                    // TODO: Temp - this should be handled by Filesystem
                    std::filesystem::path path = filename;
                    auto parentPath = path.parent_path();
                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();
                    ET_MESH_LOG("    Roughness map path = {0}", texturePath);
                    auto texture = AssetManager::GetAsset<Texture>(texturePath);
                    if (texture->IsLoaded()) {
                        mi->SetRoughnessMap(texture);
                        mi->SetRoughness(1.0f);
                    } else {
                        ET_CORE_ERROR("    Could not load texture: {0}", texturePath);
                        fallback = true;
                    }
                }

                if (fallback) {
                    ET_MESH_LOG("    No roughness map");
                    mi->ClearRoughnessMap();
                    mi->SetRoughness(roughness);
                }

                bool metalnessTextureFound = false;
                for (uint32_t p = 0; p < aiMaterial->mNumProperties; p++) {
                    auto prop = aiMaterial->mProperties[p];

                    if (prop->mType == aiPTI_String) {
                        uint32_t strLength = *(uint32_t*)prop->mData;
                        std::string str(prop->mData + 4, strLength);

                        std::string key = prop->mKey.data;
                        if (key == "$raw.ReflectionFactor|file") {
                            // TODO: Temp - this should be handled by Filesystem
                            std::filesystem::path path = filename;
                            auto parentPath = path.parent_path();
                            parentPath /= str;
                            std::string texturePath = parentPath.string();
                            ET_MESH_LOG("    Metalness map path = {0}", texturePath);
                            auto texture = AssetManager::GetAsset<Texture>(texturePath);
                            if (texture->IsLoaded()) {
                                metalnessTextureFound = true;
                                mi->SetMetalnessMap(texture);
                                mi->SetMetalness(1.0f);
                            } else {
                                ET_CORE_ERROR("    Could not load texture: {0}", texturePath);
                            }
                            break;
                        }
                    }
                }
                mi->SetUseMetalnessMap(metalnessTextureFound);
                fallback = !metalnessTextureFound;
                if (fallback) {
                    ET_MESH_LOG("    No metalness map");
                    mi->ClearMetalnessMap();
                    mi->SetMetalness(metalness);
                }
            }
            ET_MESH_LOG("------------------------");
        } else {
            auto mi = Ref<MaterialAsset>::Create("Ethereal-Default");
            m_Materials.push_back(mi);
        }

        m_VertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), (uint32_t)(m_StaticVertices.size() * sizeof(Vertex)));
        m_VertexBufferLayout = {
            {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
            {ShaderDataType::Float3, "a_Binormal"}, {ShaderDataType::Float2, "a_TexCoord"},
        };

        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));

        m_VertexArray = VertexArray::Create();
        m_VertexBuffer->SetLayout(m_VertexBufferLayout);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
    }

    void MeshSource::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level) {
        glm::mat4 localTransform = Utils::Mat4FromAssimpMat4(node->mTransformation);
        glm::mat4 transform = parentTransform * localTransform;
        m_NodeMap[node].resize(node->mNumMeshes);
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            uint32_t mesh = node->mMeshes[i];
            auto& submesh = m_Submeshes[mesh];
            submesh.NodeName = node->mName.C_Str();
            submesh.Transform = transform;
            submesh.LocalTransform = localTransform;
            m_NodeMap[node][i] = mesh;
        }

        // ET_MESH_LOG("{0} {1}", LevelToSpaces(level), node->mName.C_Str());

        for (uint32_t i = 0; i < node->mNumChildren; i++) TraverseNodes(node->mChildren[i], transform, level + 1);
    }

    MeshSource::MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const glm::mat4& transform)
        : m_StaticVertices(vertices), m_Indices(indices) {
        // Generate a new asset handle
        Handle = {};

        Submesh submesh;
        submesh.BaseVertex = 0;
        submesh.BaseIndex = 0;
        submesh.IndexCount = (uint32_t)indices.size() * 3u;
        submesh.Transform = transform;
        m_Submeshes.push_back(submesh);

        m_VertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), (uint32_t)(m_StaticVertices.size() * sizeof(Vertex)));
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));
        m_VertexBufferLayout = {
            {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
            {ShaderDataType::Float3, "a_Binormal"}, {ShaderDataType::Float2, "a_TexCoord"},
        };

        m_VertexArray = VertexArray::Create();
        m_VertexBuffer->SetLayout(m_VertexBufferLayout);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
    }

    MeshSource::MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const std::vector<Submesh>& submeshes)
        : m_StaticVertices(vertices), m_Indices(indices), m_Submeshes(submeshes) {
        // Generate a new asset handle
        Handle = {};

        m_VertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), (uint32_t)(m_StaticVertices.size() * sizeof(Vertex)));
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));
        m_VertexBufferLayout = {
            {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
            {ShaderDataType::Float3, "a_Binormal"}, {ShaderDataType::Float2, "a_TexCoord"},
        };

        m_VertexArray = VertexArray::Create();
        m_VertexBuffer->SetLayout(m_VertexBufferLayout);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
        // TODO: generate bounding box for submeshes, etc.
    }

    MeshSource::~MeshSource() {}

    static std::string LevelToSpaces(uint32_t level) {
        std::string result = "";
        for (uint32_t i = 0; i < level; i++) result += "--";
        return result;
    }

    StaticMesh::StaticMesh(Ref<MeshSource> meshSource) : m_MeshSource(meshSource) {
        // Generate a new asset handle
        Handle = {};

        SetSubmeshes({});

        const auto& meshMaterials = meshSource->GetMaterials();
        m_Materials = Ref<MaterialTable>::Create(meshMaterials.size());
        for (size_t i = 0; i < meshMaterials.size(); i++) m_Materials->SetMaterial(i, Ref<MaterialAsset>::Create(meshMaterials[i]));
    }

    StaticMesh::StaticMesh(Ref<MeshSource> meshSource, const std::vector<uint32_t>& submeshes) : m_MeshSource(meshSource) {
        // Generate a new asset handle
        Handle = {};

        SetSubmeshes(submeshes);

        const auto& meshMaterials = meshSource->GetMaterials();
        m_Materials = Ref<MaterialTable>::Create(meshMaterials.size());
        for (size_t i = 0; i < meshMaterials.size(); i++) m_Materials->SetMaterial(i, meshMaterials[i]);
    }

    StaticMesh::StaticMesh(const Ref<StaticMesh>& other) : m_MeshSource(other->m_MeshSource), m_Materials(other->m_Materials) {
        SetSubmeshes(other->m_Submeshes);
    }

    StaticMesh::~StaticMesh() {}

    void StaticMesh::SetSubmeshes(const std::vector<uint32_t>& submeshes) {
        if (!submeshes.empty()) {
            m_Submeshes = submeshes;
        } else {
            const auto& submeshes = m_MeshSource->GetSubmeshes();
            m_Submeshes.resize(submeshes.size());
            for (uint32_t i = 0; i < submeshes.size(); i++) m_Submeshes[i] = i;
        }
    }
}  // namespace Ethereal