#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

namespace mokm {

class ImageBuffer;

class OCIOWrapper {
public:
    static OCIOWrapper& instance();

    bool loadConfig(const std::string& path);
    bool loadConfigFromString(const std::string& xml);

    OCIO::ConstConfigRcPtr config() const { return m_config; }
    bool isLoaded() const { return m_config != nullptr; }

    OCIO::ConstProcessorRcPtr getProcessor(
        const std::string& srcColorSpace,
        const std::string& dstColorSpace
    );

    OCIO::ConstProcessorRcPtr getProcessor(
        const std::string& srcColorSpace,
        const std::string& dstColorSpace,
        const OCIO::ContextRcPtr& context
    );

    OCIO::ConstProcessorRcPtr getDisplayProcessor(
        const std::string& display,
        const std::string& view,
        const std::string& inputColorSpace
    );

    bool apply(OCIO::ConstProcessorRcPtr processor, ImageBuffer& buffer);

    std::vector<std::string> colorSpaces() const;
    std::vector<std::string> displays() const;
    std::vector<std::string> views(const std::string& display) const;
    std::vector<std::string> looks() const;

    void clearCache();
    void setContextKey(const std::string& key, const std::string& value);

    std::string lastError() const { return m_lastError; }

private:
    OCIOWrapper() = default;
    ~OCIOWrapper() = default;
    OCIOWrapper(const OCIOWrapper&) = delete;
    OCIOWrapper& operator=(const OCIOWrapper&) = delete;

    struct ProcessorKey {
        std::string src;
        std::string dst;
        std::string contextStr;
        bool operator==(const ProcessorKey& o) const {
            return src == o.src && dst == o.dst && contextStr == o.contextStr;
        }
    };
    struct ProcessorKeyHash {
        size_t operator()(const ProcessorKey& k) const {
            return std::hash<std::string>()(k.src) ^
                   std::hash<std::string>()(k.dst) ^
                   std::hash<std::string>()(k.contextStr);
        }
    };

    OCIO::ConstConfigRcPtr m_config;
    OCIO::ContextRcPtr m_context = OCIO::Context::Create();
    std::unordered_map<ProcessorKey, OCIO::ConstProcessorRcPtr, ProcessorKeyHash> m_cache;
    std::string m_lastError;
};

} // namespace mokm
