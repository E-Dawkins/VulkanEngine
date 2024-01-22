#pragma once
#include "Transform.h"

class Mesh
{
public:
    Mesh(std::string _meshPath, std::string _texturePath);

    void DrawMesh(VkCommandBuffer _commandBuffer) const;
    void Cleanup() const;
    
    VkImageView GetTextureImageView() const { return m_textureImageView; }
    VkSampler GetTextureSampler() const { return m_textureSampler; }

private:
    void CreateTextureImage();
    static void GenerateMipmaps(VkImage _image, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight,
                                uint32_t _mipLevels);
    void CreateTextureImageView();
    void CreateTextureSampler();
    void LoadModel();
    void CreateVertexBuffer();
    void CreateIndexBuffer();

public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    
        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription;
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
            return bindingDescription;
        }
    
        static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        
            return attributeDescriptions;
        }
    
        bool operator==(const Vertex& _other) const
        {
            return pos == _other.pos && color == _other.color && texCoord == _other.texCoord;
        }
    };

    Transform transform = Transform();
    
private:
    std::string m_meshPath;
    std::string m_texturePath;

    VkBuffer m_vertexBuffer{};
    VkDeviceMemory m_vertexBufferMemory{};
    VkBuffer m_indexBuffer{};
    VkDeviceMemory m_indexBufferMemory{};

    uint32_t m_mipLevels{};
    VkImage m_textureImage{};
    VkDeviceMemory m_textureImageMemory{};
    VkImageView m_textureImageView{};
    VkSampler m_textureSampler{};

    std::vector<Vertex> m_vertices{};
    std::vector<uint32_t> m_indices{};
};

template<> struct std::hash<Mesh::Vertex>
{
    size_t operator()(Mesh::Vertex const& _vertex) const noexcept
    {
        return (hash<glm::vec3>()(_vertex.pos) ^
                hash<glm::vec3>()(_vertex.color) << 1) >> 1 ^
                hash<glm::vec2>()(_vertex.texCoord) << 1;
    }
};
