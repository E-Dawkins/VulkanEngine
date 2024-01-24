#pragma once
#include <optional>
#include <vector>

class Material_Base;
class Mesh;

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

class Renderer
{
public:
    Renderer(int _width, int _height, int _maxFramesInFlight = 2);
    void Initialize();

    static VkDevice GetDevice() { return m_device; }
    static VkPhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }
    static float GetSwapchainRatio() { return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height); }
    static VkExtent2D GetSwapchainExtent() { return m_swapChainExtent; }
    static VkSampleCountFlagBits GetMsaaSampleCount() { return m_msaaSamples; }

    static void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory);
    static uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties);
    static void CreateImage(uint32_t _width, uint32_t _height, uint32_t _mipLevels, VkSampleCountFlagBits _numSamples, VkFormat _format,
                     VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkImage& _image, VkDeviceMemory& _imageMemory);
    static void TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _mipLevels);
    static VkCommandBuffer BeginSingleTimeCommands();
    static void EndSingleTimeCommands(VkCommandBuffer _commandBuffer);
    static VkImageView CreateImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, uint32_t _mipLevels);
    static void CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height);
    static void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);

private:
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void Cleanup() const;
    
    void InitMaterials();
    void InitMeshes();
    
    void CleanupResources() const;

    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevices();
    void CreateSwapChain();
    void RecreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateCommandPool() const;
    void CreateColorResources();
    void CreateDepthResources();
    void CreateFrameBuffers();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    void DrawFrame();
    void RecordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex) const;
    void CleanupSwapChain() const;
    void UpdateUniformBuffer(uint32_t _currentImage) const;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities) const;
    static VkFormat FindSupportedFormat(const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features);
    static VkFormat FindDepthFormat();
    bool IsDeviceSuitable(VkPhysicalDevice _device) const;
    static VkSampleCountFlagBits GetMaxUsableSampleCount();
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
    
private:
    int m_width = 800;
    int m_height = 600;
    int m_maxFramesInFlight = 2;
    bool m_frameBufferResized = false;

    GLFWwindow* m_window = nullptr;

    VkInstance m_instance{};
    VkDebugUtilsMessengerEXT m_debugMessenger{};
    inline static VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    inline static VkDevice m_device{};
    inline static VkQueue m_graphicsQueue{};
    VkSurfaceKHR m_surface{};
    VkQueue m_presentQueue{};
    VkSwapchainKHR m_swapChain{};
    std::vector<VkImage> m_swapChainImages{};
    VkFormat m_swapChainImageFormat{};
    inline static VkExtent2D m_swapChainExtent{};
    std::vector<VkImageView> m_swapChainImageViews{};
    VkRenderPass m_renderPass{};
    std::vector<VkFramebuffer> m_swapChainFrameBuffers{};
    inline static VkCommandPool m_commandPool{};
    std::vector<VkCommandBuffer> m_commandBuffers{};
    std::vector<VkSemaphore> m_imageAvailableSemaphores{};
    std::vector<VkSemaphore> m_renderFinishedSemaphores{};
    std::vector<VkFence> m_inFlightFences{};

    inline static VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    VkImage m_depthImage{};
    VkDeviceMemory m_depthImageMemory{};
    VkImageView m_depthImageView{};

    VkImage m_colorImage{};
    VkDeviceMemory m_colorImageMemory{};
    VkImageView m_colorImageView{};
    
    uint32_t m_currentFrame = 0;
    
    Mesh* m_mesh = nullptr;
    Material_Base* m_material = nullptr;
};
