#pragma once

class Texture
{
public:
    Texture(std::string _texturePath);
    ~Texture();

    VkImageView& GetImageView() { return m_textureImageView; }
    VkSampler& GetSampler() { return m_textureSampler; }
    
private:
    void CreateTextureImage();
    static void GenerateMipmaps(VkImage _image, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight,
                                uint32_t _mipLevels);
    void CreateTextureImageView();
    void CreateTextureSampler();
    
private:
    std::string m_texturePath;
    
    uint32_t m_mipLevels{};
    VkImage m_textureImage{};
    VkDeviceMemory m_textureImageMemory{};
    VkImageView m_textureImageView{};
    VkSampler m_textureSampler{};
};
