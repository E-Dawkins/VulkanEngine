#pragma once

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void InitMesh(const std::string& _meshPath, bool _logInitTime = false);
    void DrawMesh(VkCommandBuffer _commandBuffer) const;

private:
    void LoadModel();
    void CreateVertexBuffer();
    void CreateIndexBuffer();

public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 texCoord;
    
        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription;
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
            return bindingDescription;
        }
    
        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
        
            return attributeDescriptions;
        }
    
        bool operator==(const Vertex& _other) const
        {
            return pos == _other.pos && texCoord == _other.texCoord;
        }
    };
    
private:
    std::string m_meshPath;

    std::vector<Vertex> m_vertices{};
    VkBuffer m_vertexBuffer{};
    VkDeviceMemory m_vertexBufferMemory{};
    
    std::vector<uint32_t> m_indices{};
    VkBuffer m_indexBuffer{};
    VkDeviceMemory m_indexBufferMemory{};
};

// Hash implementation - needed to use Mesh::Vertex within an unordered set / map
template <>
struct std::hash<Mesh::Vertex>
{
    size_t operator()(const Mesh::Vertex& _other) const noexcept
    {
        const size_t pHash = std::hash<glm::vec3>()(_other.pos);
        const size_t tHash = std::hash<glm::vec2>()(_other.texCoord) << 1;
        return pHash ^ tHash;
    }
};
