#include "ObjLoader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
namespace Ethereal
{
    void ObjLoader::Load(const GameObjectMeshDesc& desc, Ref<BufferData>& VertexBuffer, Ref<BufferData>& IndexBuffer) {
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

        size_t vertices_num = attrib.vertices.size() / 3;
        
        // for (size_t i = 0; i < vertices_num; i++) {
        //     MeshVertexPtr[i].Position = glm::vec3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);
        //     MeshVertexPtr[i].Normal = glm::vec3(attrib.normals[3 * i], attrib.normals[3 * i + 1], attrib.normals[3 * i + 2]);
        //     MeshVertexPtr[i].TexCoord = glm::vec2(attrib.texcoords[2 * i], attrib.texcoords[2 * i + 1]);
        //     // MeshVertexPtr[i].Color = glm::vec4(attrib.colors[3 * i], attrib.colors[3 * i + 1], attrib.colors[3 * i + 2], 1.0f);
        // }
        size_t index_num = 0;
        for (size_t s = 0; s < shapes.size(); s++) {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                index_num += size_t(shapes[s].mesh.num_face_vertices[f]);
            }
        }
        VertexBuffer = CreateRef<BufferData>(index_num * sizeof(MeshVertex));
        MeshVertex* MeshVertexPtr = (MeshVertex*)VertexBuffer->m_data;
        IndexBuffer = CreateRef<BufferData>(index_num * sizeof(uint32_t));
        uint32_t* IndexBufferPtr = (uint32_t*)IndexBuffer->m_data;

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                size_t material_id =  shapes[s].mesh.material_ids[f];
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    MeshVertexPtr->Position = glm::vec3(vx/100.0, vy/100.0, vz/100.0);
                    *IndexBufferPtr = idx.vertex_index;
                    IndexBufferPtr++;
                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        MeshVertexPtr->Normal = glm::vec3(nx, ny, nz);
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        MeshVertexPtr->TexCoord = glm::vec2(tx, ty);
                    }

                    MeshVertexPtr->Color = glm::vec4(materials[material_id].diffuse[0], materials[material_id].diffuse[1], materials[material_id].diffuse[2],1.0);
                    MeshVertexPtr++;
                    //ET_CORE_INFO("diffuse {0} {1} {2}",materials[material_id].diffuse[0], materials[material_id].diffuse[1], materials[material_id].diffuse[2]);
                    // Optional: vertex colors
                    // tinyobj::real_t red = attrib.colors[3 * size_t(idx.vertex_index) + 0];
                    // tinyobj::real_t green = attrib.colors[3 * size_t(idx.vertex_index) + 1];
                    // tinyobj::real_t blue = attrib.colors[3 * size_t(idx.vertex_index) + 2];
                }
                index_offset += fv;

                //ET_CORE_INFO("material id: {0}", shapes[s].mesh.material_ids[f]);
            }
        }
    }
}  // namespace Ethereal