#include "mokm/ImageBuffer.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

namespace mokm {

ImageBuffer::ImageBuffer()
    : m_width(0), m_height(0), m_channels(4), m_format(PixelFormat::F32)
{
}

ImageBuffer::ImageBuffer(int width, int height, int channels, PixelFormat fmt)
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_format(fmt)
{
    if (fmt != PixelFormat::F32)
        m_format = PixelFormat::F32;
    allocate();
}

void ImageBuffer::allocate()
{
    if (m_width > 0 && m_height > 0 && m_channels > 0) {
        m_data.resize(static_cast<size_t>(m_width) * m_height * m_channels);
        std::fill(m_data.begin(), m_data.end(), 0.0f);
    }
}

void ImageBuffer::clear(float value)
{
    std::fill(m_data.begin(), m_data.end(), value);
}

bool ImageBuffer::loadFromFile(const std::string& path)
{
    auto ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "ppm") {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        std::string header;
        int maxVal;
        f >> header >> m_width >> m_height >> maxVal;
        f.get();
        m_channels = (header == "P6") ? 3 : 1;
        allocate();
        if (maxVal == 255) {
            std::vector<uint8_t> tmp(m_data.size());
            f.read(reinterpret_cast<char*>(tmp.data()), tmp.size());
            for (size_t i = 0; i < m_data.size(); ++i)
                m_data[i] = tmp[i] / 255.0f;
        } else {
            f.read(reinterpret_cast<char*>(m_data.data()), m_data.size() * sizeof(float));
        }
        return true;
    }

    return false;
}

bool ImageBuffer::saveToFile(const std::string& path) const
{
    auto ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "ppm" || ext == "ppm") {
        std::ofstream f(path, std::ios::binary);
        if (!f) return false;
        int chan = (std::min)(m_channels, 3);
        f << "P6\n" << m_width << " " << m_height << "\n255\n";
        std::vector<uint8_t> tmp(static_cast<size_t>(m_width) * m_height * chan);
        for (size_t i = 0; i < tmp.size(); ++i)
            tmp[i] = static_cast<uint8_t>(std::clamp(m_data[i] * 255.0f, 0.0f, 255.0f));
        f.write(reinterpret_cast<char*>(tmp.data()), tmp.size());
        return true;
    }

    return false;
}

ImageBuffer ImageBuffer::roi(int x, int y, int w, int h) const
{
    ImageBuffer result(w, h, m_channels, m_format);
    for (int row = 0; row < h; ++row) {
        int srcRow = y + row;
        if (srcRow < 0 || srcRow >= m_height) continue;
        size_t srcOff = static_cast<size_t>(srcRow) * m_width * m_channels + static_cast<size_t>(x) * m_channels;
        size_t dstOff = static_cast<size_t>(row) * w * m_channels;
        size_t copyBytes = static_cast<size_t>(std::min(w, m_width - x)) * m_channels;
        if (copyBytes > 0)
            std::memcpy(&result.m_data[dstOff], &m_data[srcOff], copyBytes * sizeof(float));
    }
    return result;
}

ImageBuffer ImageBuffer::clone() const
{
    ImageBuffer result(m_width, m_height, m_channels, m_format);
    result.m_data = m_data;
    result.m_colorSpace = m_colorSpace;
    return result;
}

bool ImageBuffer::supportsFormat(const std::string& ext)
{
    std::string lower = ext;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "ppm";
}

} // namespace mokm
