#pragma once
#include <array>
#include <optional>
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";

const std::vector VALIDATION_LAYERS =
{
    "VK_LAYER_KHRONOS_validation"
};

const std::vector DEVICE_EXTENSIONS =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

inline VkResult CreateDebugUtilsMessengerExt(const VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo,
                                             const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT* _pDebugMessenger)
{
    if (const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT")))
    {
        return func(_instance, _pCreateInfo, _pAllocator, _pDebugMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

inline void DestroyDebugUtilsMessengerExt(const VkInstance _instance, const VkDebugUtilsMessengerEXT _debugMessenger,
                                          const VkAllocationCallbacks* _pAllocator)
{
    if (const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT")))
    {
        func(_instance, _debugMessenger, _pAllocator);
    }
}

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
 
    bool IsComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

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

template<> struct std::hash<Vertex>
{
    size_t operator()(Vertex const& _vertex) const noexcept
    {
        return (hash<glm::vec3>()(_vertex.pos) ^
                hash<glm::vec3>()(_vertex.color) << 1) >> 1 ^
                hash<glm::vec2>()(_vertex.texCoord) << 1;
    }
};

class Renderer
{
public:
    Renderer(int _width, int _height, int _maxFramesInFlight = 2);
    void Initialize();

private:
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void Cleanup() const;

    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevices();
    void CreateSwapChain();
    void RecreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateCommandPool();
    void CreateColorResources();
    void CreateDepthResources();
    void CreateFrameBuffers();
    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();
    void LoadModel();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    void GenerateMipmaps(VkImage _image, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels) const;
    void CreateImage(uint32_t _width, uint32_t _height, uint32_t _mipLevels, VkSampleCountFlagBits _numSamples, VkFormat _format,
                     VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkImage& _image, VkDeviceMemory& _imageMemory) const;
    void TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _mipLevels) const;
    void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory) const;
    void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size) const;
    void CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height) const;
    void DrawFrame();
    void RecordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex) const;
    void CleanupSwapChain() const;
    void UpdateUniformBuffer(uint32_t _currentImage) const;
    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer _commandBuffer) const;
    uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties) const;
    VkShaderModule CreateShaderModule(const std::vector<char>& _code) const;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities) const;
    VkImageView CreateImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, uint32_t _mipLevels) const;
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features) const;
    VkFormat FindDepthFormat() const;
    bool IsDeviceSuitable(VkPhysicalDevice _device) const;
    VkSampleCountFlagBits GetMaxUsableSampleCount() const;
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _device) const;
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice _device) const;

    static bool HasStencilComponent(VkFormat _format);
    static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);
    static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
    static bool CheckDeviceExtensionSupport(VkPhysicalDevice _device);
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo);
    static std::vector<const char*> GetRequiredExtensions();
    static bool CheckValidationLayerSupport();
    static void FrameBufferResizeCallback(GLFWwindow* _window, int _width, int _height);
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, VkDebugUtilsMessageTypeFlagsEXT _messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData);
    static std::vector<char> ReadFile(const std::string& _filename);
    
private:
    int m_width = 800;
    int m_height = 600;
    int m_maxFramesInFlight = 2;
    bool m_frameBufferResized = false;

    GLFWwindow* m_window = nullptr;

    VkInstance m_instance{};
    VkDebugUtilsMessengerEXT m_debugMessenger{};
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device{};
    VkQueue m_graphicsQueue{};
    VkSurfaceKHR m_surface{};
    VkQueue m_presentQueue{};
    VkSwapchainKHR m_swapChain{};
    std::vector<VkImage> m_swapChainImages{};
    VkFormat m_swapChainImageFormat{};
    VkExtent2D m_swapChainExtent{};
    std::vector<VkImageView> m_swapChainImageViews{};
    VkRenderPass m_renderPass{};
    VkDescriptorSetLayout m_descriptorSetLayout{};
    VkPipelineLayout m_pipelineLayout{};
    VkPipeline m_graphicsPipeline{};
    std::vector<VkFramebuffer> m_swapChainFrameBuffers{};
    VkCommandPool m_commandPool{};
    std::vector<VkCommandBuffer> m_commandBuffers{};
    std::vector<VkSemaphore> m_imageAvailableSemaphores{};
    std::vector<VkSemaphore> m_renderFinishedSemaphores{};
    std::vector<VkFence> m_inFlightFences{};
    
    VkBuffer m_vertexBuffer{};
    VkDeviceMemory m_vertexBufferMemory{};
    VkBuffer m_indexBuffer{};
    VkDeviceMemory m_indexBufferMemory{};

    VkImage m_depthImage{};
    VkDeviceMemory m_depthImageMemory{};
    VkImageView m_depthImageView{};

    VkImage m_colorImage{};
    VkDeviceMemory m_colorImageMemory{};
    VkImageView m_colorImageView{};

    std::vector<VkBuffer> m_uniformBuffers{};
    std::vector<VkDeviceMemory> m_uniformBuffersMemory{};
    std::vector<void*> m_uniformBuffersMapped{};
    VkDescriptorPool m_descriptorPool{};
    std::vector<VkDescriptorSet> m_descriptorSets{};

    uint32_t m_mipLevels{};
    VkImage m_textureImage{};
    VkDeviceMemory m_textureImageMemory{};
    VkImageView m_textureImageView{};
    VkSampler m_textureSampler{};
    VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    
    uint32_t m_currentFrame = 0;

    std::vector<Vertex> m_vertices{};
    std::vector<uint32_t> m_indices{};

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
};
