#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace mokm {

enum class PixelFormat {
    Unknown,
    U8,      // uint8 per channel
    F32,     // float per channel
};

class ImageBuffer {
public:
    ImageBuffer();
    ImageBuffer(int width, int height, int channels, PixelFormat fmt = PixelFormat::F32);
    ~ImageBuffer() = default;

    ImageBuffer(const ImageBuffer&) = delete;
    ImageBuffer& operator=(const ImageBuffer&) = delete;
    ImageBuffer(ImageBuffer&&) noexcept = default;
    ImageBuffer& operator=(ImageBuffer&&) noexcept = default;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    PixelFormat format() const { return m_format; }
    int64_t sizeBytes() const { return m_data.size(); }

    float* data() { return m_data.data(); }
    const float* data() const { return m_data.data(); }
    size_t dataSize() const { return m_data.size(); }

    std::string colorSpace() const { return m_colorSpace; }
    void setColorSpace(const std::string& cs) { m_colorSpace = cs; }

    void allocate();
    void clear(float value = 0.0f);
    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

    ImageBuffer roi(int x, int y, int w, int h) const;
    ImageBuffer clone() const;

    static bool supportsFormat(const std::string& ext);

private:
    int m_width = 0;
    int m_height = 0;
    int m_channels = 4;
    PixelFormat m_format = PixelFormat::F32;
    std::vector<float> m_data;
    std::string m_colorSpace;
};

using ImageBufferPtr = std::shared_ptr<ImageBuffer>;

} // namespace mokm
