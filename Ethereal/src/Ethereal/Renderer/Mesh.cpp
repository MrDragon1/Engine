#include "pch.hpp"
#include "Mesh.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "imgui.h"

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

        m_MeshShader = Renderer::GetShaderLibrary()->Get("HazelPBR_Static");
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

            // Vertices
            if (m_IsAnimated) {
                for (size_t i = 0; i < mesh->mNumVertices; i++) {
                    AnimatedVertex vertex;
                    vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
                    vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

                    if (mesh->HasTangentsAndBitangents()) {
                        vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                        vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                    }

                    if (mesh->HasTextureCoords(0)) vertex.Texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

                    m_AnimatedVertices.push_back(vertex);
                }
            } else {
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

                    if (mesh->HasTextureCoords(0)) vertex.Texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

                    m_StaticVertices.push_back(vertex);
                }
            }

            // Indices
            for (size_t i = 0; i < mesh->mNumFaces; i++) {
                ET_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
                Index index = {mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]};
                m_Indices.push_back(index);

                if (!m_IsAnimated)
                    m_TriangleCache[m].emplace_back(m_StaticVertices[index.V1 + submesh.BaseVertex], m_StaticVertices[index.V2 + submesh.BaseVertex],
                                                    m_StaticVertices[index.V3 + submesh.BaseVertex]);
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
        Ref<Texture2D> whiteTexture = Renderer::GetWhiteTexture();
        if (scene->HasMaterials()) {
            ET_MESH_LOG("---- Materials - {0} ----", filename);

            m_Textures.resize(scene->mNumMaterials);
            m_Materials.resize(scene->mNumMaterials);
            ET_MESH_LOG("------------------------");
        } else {
            auto mi = Material::Create(m_MeshShader, "Hazel-Default");
            mi->Set("u_MaterialUniforms.AlbedoColor", glm::vec3(0.8f));
            mi->Set("u_MaterialUniforms.Emission", 0.0f);
            mi->Set("u_MaterialUniforms.Metalness", 0.0f);
            mi->Set("u_MaterialUniforms.Roughness", 0.8f);
            mi->Set("u_MaterialUniforms.UseNormalMap", false);

            mi->Set("u_AlbedoTexture", whiteTexture);
            mi->Set("u_MetalnessTexture", whiteTexture);
            mi->Set("u_RoughnessTexture", whiteTexture);
            m_Materials.push_back(mi);
        }

        if (m_IsAnimated) {
            m_VertexBuffer = VertexBuffer::Create(m_AnimatedVertices.data(), (uint32_t)(m_AnimatedVertices.size() * sizeof(AnimatedVertex)));
            m_VertexBufferLayout = {
                {ShaderDataType::Float3, "a_Position"},    {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
                {ShaderDataType::Float3, "a_Binormal"},    {ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int4, "a_BoneIDs"},
                {ShaderDataType::Float4, "a_BoneWeights"},
            };
        } else {
            m_VertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), (uint32_t)(m_StaticVertices.size() * sizeof(Vertex)));
            m_VertexBufferLayout = {
                {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},   {ShaderDataType::Float3, "a_Tangent"},
                {ShaderDataType::Float3, "a_Binormal"}, {ShaderDataType::Float2, "a_TexCoord"},
            };
        }

        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));
    }
}  // namespace Ethereal