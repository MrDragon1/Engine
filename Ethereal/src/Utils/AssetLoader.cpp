#include "pch.h"
#include "AssetLoader.h"
#include "stb_image.h"

namespace Ethereal
{
    std::vector<MeshVertex> ObjLoader::m_MeshVertices;
    std::vector<unsigned int> ObjLoader::m_MeshIndices;

    void ObjLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            MeshVertex vertex;
            glm::vec3 vector;  // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3
                               // class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.UV = vec;
                // // tangent
                // vector.x = mesh->mTangents[i].x;
                // vector.y = mesh->mTangents[i].y;
                // vector.z = mesh->mTangents[i].z;
                // vertex.Tangent = vector;
                // // bitangent
                // vector.x = mesh->mBitangents[i].x;
                // vector.y = mesh->mBitangents[i].y;
                // vector.z = mesh->mBitangents[i].z;
                // vertex.Bitangent = vector;
            } else
                vertex.UV = glm::vec2(0.0f, 0.0f);

            m_MeshVertices.push_back(vertex);
        }

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++) m_MeshIndices.push_back(face.mIndices[j]);
        }
    }

    void ObjLoader::processNode(aiNode* node, const aiScene* scene) {
        // 处理节点所有的网格（如果有的话）
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
        // 接下来对它的子节点重复这一过程
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }
    void ObjLoader::Load(const GameObjectMeshDesc& desc, RenderMeshData& renderMeshData) {
        m_MeshVertices.clear();
        m_MeshIndices.clear();
        Assimp::Importer importer;

        std::string inputfile = desc.m_filePath;
        const aiScene* scene = importer.ReadFile(inputfile, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            ET_CORE_ERROR("ObjLoader::Load Error: {0} when load {1}", importer.GetErrorString(), inputfile);
            return;
        }

        processNode(scene->mRootNode, scene);

        renderMeshData.m_static_mesh_data.m_vertex_buffer = Ref<BufferData<MeshVertex>>::Create(m_MeshVertices.size());
        renderMeshData.m_static_mesh_data.m_index_buffer = Ref<BufferData<uint32_t>>::Create(m_MeshIndices.size());

        assert(m_MeshVertices.size() <= std::numeric_limits<uint32_t>::max());  // take care of the index range, should be
                                                                                // consistent with the index range used by
                                                                                // vulkan
        uint32_t* indices = (uint32_t*)renderMeshData.m_static_mesh_data.m_index_buffer->m_data;
        for (size_t i = 0; i < m_MeshVertices.size(); i++) {
            ((MeshVertex*)(renderMeshData.m_static_mesh_data.m_vertex_buffer->m_data))[i] = m_MeshVertices[i];
            // indices[i] = static_cast<uint32_t>(i);
        }
        for(size_t i = 0; i < m_MeshIndices.size(); i++) {
            indices[i] = m_MeshIndices[i];
        }
    }

    void TextureLoader::LoadPath(const std::string& path, Ref<TextureData>& textureData, bool flip) {
        stbi_set_flip_vertically_on_load(flip);
        int width, height, channels;
        bool hdr = path.ends_with(".hdr");
        void* data;
        if (hdr)
            data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
        else
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data) {
            textureData->m_width = width;
            textureData->m_height = height;
            textureData->m_depth = 1;
            textureData->m_mip_levels = 1;
            textureData->m_type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D;
            size_t datasize = width * height * channels;
            if (hdr) datasize *= sizeof(float);
            textureData->m_pixels = malloc(datasize);
            memcpy(textureData->m_pixels, data, datasize);
            if (hdr) {
                textureData->m_format = ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_HDR;
            } else {
                if (channels == 4) {
                    textureData->m_format = ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM;
                } else if (channels == 3) {
                    textureData->m_format = ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8_UNORM;
                } else if (channels == 1) {
                    textureData->m_format = ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_RED;
                }
            }

            stbi_image_free(data);
        } else {
            ET_CORE_WARN("Failed to load texture: {0}", path);
            textureData = nullptr;
        }
    }
}  // namespace Ethereal