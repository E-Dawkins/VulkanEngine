#pragma once

class Texture;

class TextureSampler
{
public:
    TextureSampler(const Texture* _texture);
    ~TextureSampler();

    const VkSampler& Sampler() const { return m_sampler; }

private:
    VkSampler m_sampler{};
};
