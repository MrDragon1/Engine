#include "pch.hpp"
#include "AssetLoader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "stb_image/stb_image.h"

namespace Ethereal
{
    void ObjLoader::Load(const GameObjectMeshDesc& desc, RenderMeshData& renderMeshData) {
        tinyobj::ObjReaderConfig reader_config;
        tinyobj::ObjReader* reader = new tinyobj::ObjReader();

        std::string inputfile = desc.m_filePath;
        if (!reader->ParseFromFile(inputfile, reader_config)) {
            if (!reader->Error().empty()) {
                ET_CORE_ERROR("ObjLoader::Load Error: {0} when load {1}", reader->Error(), inputfile);
            }
            exit(1);
        }

        if (!reader->Warning().empty()) {
            ET_CORE_WARN("ObjLoader: {}", reader->Warning());
        }
        auto& attrib = reader->GetAttrib();
        auto& shapes = reader->GetShapes();
        auto& materials = reader->GetMaterials();

        std::vector<MeshVertex> mesh_vertices;

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // TODO: support multiple materials for multiple shapes in one obj file
            if (s > 1) ET_CORE_WARN("Ethereal only support one shape, because only one material per object is supported");
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                size_t material_id = shapes[s].mesh.material_ids[f];

                if (fv != 3) {
                    ET_CORE_WARN("ObjLoader: Only triangle is supported");
                    continue;
                }

                glm::vec3 vertex[3];
                glm::vec3 normal[3];
                glm::vec2 uv[3];

                bool has_normal = true;
                bool has_uv = true;
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    vertex[v] = glm::vec3(vx, vy, vz);
                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        normal[v] = glm::vec3(nx, ny, nz);
                    } else
                        has_normal = false;

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        uv[v] = glm::vec2(tx, ty);
                    } else
                        has_uv = false;

                    // ET_CORE_INFO("diffuse {0} {1} {2}",materials[material_id].diffuse[0], materials[material_id].diffuse[1],
                    // materials[material_id].diffuse[2]);
                    //  Optional: vertex colors
                    //  tinyobj::real_t red = attrib.colors[3 * size_t(idx.vertex_index) + 0];
                    //  tinyobj::real_t green = attrib.colors[3 * size_t(idx.vertex_index) + 1];
                    //  tinyobj::real_t blue = attrib.colors[3 * size_t(idx.vertex_index) + 2];
                }
                index_offset += fv;

                if (!has_normal) {
                    glm::vec3 v0 = vertex[1] - vertex[0];
                    glm::vec3 v1 = vertex[2] - vertex[1];
                    normal[0] = glm::normalize(glm::cross(v0, v1));
                    normal[1] = normal[0];
                    normal[2] = normal[0];
                }

                if (!has_uv) {
                    uv[0] = glm::vec2(0.5f, 0.5f);
                    uv[1] = glm::vec2(0.5f, 0.5f);
                    uv[2] = glm::vec2(0.5f, 0.5f);
                }

                glm::vec3 tangent{1, 0, 0};
                {
                    glm::vec3 edge1 = vertex[1] - vertex[0];
                    glm::vec3 edge2 = vertex[2] - vertex[1];
                    glm::vec2 deltaUV1 = uv[1] - uv[0];
                    glm::vec2 deltaUV2 = uv[2] - uv[1];

                    auto divide = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
                    if (divide >= 0.0f && divide < 0.000001f)
                        divide = 0.000001f;
                    else if (divide < 0.0f && divide > -0.000001f)
                        divide = -0.000001f;

                    float df = 1.0f / divide;
                    tangent.x = df * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                    tangent.y = df * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                    tangent.z = df * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                    tangent = glm::normalize(tangent);
                }

                for (size_t i = 0; i < 3; i++) {
                    MeshVertex mesh_vert{};
                    mesh_vert.Position = vertex[i] / glm::vec3(1);
                    mesh_vert.Normal = normal[i];
                    mesh_vert.UV = uv[i];
                    mesh_vertices.push_back(mesh_vert);
                }

                renderMeshData.m_static_mesh_data.m_vertex_buffer = CreateRef<BufferData<MeshVertex>>(mesh_vertices.size());
                renderMeshData.m_static_mesh_data.m_index_buffer = CreateRef<BufferData<uint32_t>>(mesh_vertices.size());

                assert(mesh_vertices.size() <= std::numeric_limits<uint32_t>::max());  // take care of the index range, should be
                                                                                       // consistent with the index range used by
                                                                                       // vulkan
                uint32_t* indices = (uint32_t*)renderMeshData.m_static_mesh_data.m_index_buffer->m_data;
                for (size_t i = 0; i < mesh_vertices.size(); i++) {
                    ((MeshVertex*)(renderMeshData.m_static_mesh_data.m_vertex_buffer->m_data))[i] = mesh_vertices[i];
                    indices[i] = static_cast<uint32_t>(i);
                }
            }
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
            textureData->m_array_layers = 1;
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