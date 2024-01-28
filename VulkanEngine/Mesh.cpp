#include "pch.h"
#include "Mesh.h"

#include "Renderer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Material_Base.h"

Mesh::Mesh() = default;

Mesh::~Mesh()
{
    vkDestroyBuffer(Renderer::GetInstance()->GetDevice(), m_indexBuffer, nullptr);
    vkFreeMemory(Renderer::GetInstance()->GetDevice(), m_indexBufferMemory, nullptr);

    vkDestroyBuffer(Renderer::GetInstance()->GetDevice(), m_vertexBuffer, nullptr);
    vkFreeMemory(Renderer::GetInstance()->GetDevice(), m_vertexBufferMemory, nullptr);
}

void Mesh::InitMesh(const std::string& _meshPath, const bool _logInitTime)
{
    const auto startTime = high_resolution_clock::now();
    m_meshPath = _meshPath;
    
    LoadModel();
    CreateVertexBuffer();
    CreateIndexBuffer();

    const auto endTime = high_resolution_clock::now();

    if (_logInitTime)
    {
        const float totalTime = static_cast<int16_t>(duration<float, milliseconds::period>(endTime - startTime).count());
        std::cout << "Mesh at \"" << m_meshPath << "\" loaded in " << totalTime << "ms" << std::endl;
    }

    // Add draw call to renderer pass
    Renderer::GetInstance()->AddDrawCall([this](const VkCommandBuffer _commandBuffer) {DrawMesh(_commandBuffer);});
}

void Mesh::DrawMesh(const VkCommandBuffer _commandBuffer) const
{
    material->RenderMaterial(_commandBuffer);
    
    const VkBuffer vertexBuffers[] = {m_vertexBuffer};
    constexpr VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(_commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(_commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
}

void Mesh::UpdateMesh(const float _deltaTime)
{
    material->ubo.model = transform.GetWorldMatrix();
    material->UpdateMaterial();
}

void Mesh::LoadModel()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, m_meshPath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.pos =
            {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord =
            {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            if (!uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                m_vertices.push_back(vertex);
            }
            
            m_indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void Mesh::CreateVertexBuffer()
{
    const VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Renderer::GetInstance()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Renderer::GetInstance()->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(Renderer::GetInstance()->GetDevice(), stagingBufferMemory);

    Renderer::GetInstance()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

    Renderer::GetInstance()->CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

    vkDestroyBuffer(Renderer::GetInstance()->GetDevice(), stagingBuffer, nullptr);
    vkFreeMemory(Renderer::GetInstance()->GetDevice(), stagingBufferMemory, nullptr);
}

void Mesh::CreateIndexBuffer()
{
    const VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Renderer::GetInstance()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Renderer::GetInstance()->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_indices.data(), (size_t)bufferSize);
    vkUnmapMemory(Renderer::GetInstance()->GetDevice(), stagingBufferMemory);

    Renderer::GetInstance()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

    Renderer::GetInstance()->CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

    vkDestroyBuffer(Renderer::GetInstance()->GetDevice(), stagingBuffer, nullptr);
    vkFreeMemory(Renderer::GetInstance()->GetDevice(), stagingBufferMemory, nullptr);
}
