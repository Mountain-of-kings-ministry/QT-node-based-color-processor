#include "mokm/OCIOWrapper.h"
#include "mokm/ImageBuffer.h"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace mokm {

OCIOWrapper& OCIOWrapper::instance()
{
    static OCIOWrapper inst;
    return inst;
}

bool OCIOWrapper::loadConfig(const std::string& path)
{
    try {
        m_config = OCIO::Config::CreateFromFile(path.c_str());
        m_cache.clear();
        m_lastError.clear();
        return true;
    } catch (const OCIO::Exception& e) {
        m_lastError = e.what();
        std::cerr << "OCIO loadConfig failed: " << e.what() << std::endl;
        return false;
    }
}

bool OCIOWrapper::loadConfigFromString(const std::string& xml)
{
    try {
        std::stringstream ss(xml);
        m_config = OCIO::Config::CreateFromStream(ss);
        m_cache.clear();
        m_lastError.clear();
        return true;
    } catch (const OCIO::Exception& e) {
        m_lastError = e.what();
        std::cerr << "OCIO loadConfigFromString failed: " << e.what() << std::endl;
        return false;
    }
}

OCIO::ConstProcessorRcPtr OCIOWrapper::getProcessor(
    const std::string& srcColorSpace, const std::string& dstColorSpace)
{
    return getProcessor(srcColorSpace, dstColorSpace, nullptr);
}

OCIO::ConstProcessorRcPtr OCIOWrapper::getProcessor(
    const std::string& srcColorSpace, const std::string& dstColorSpace,
    const OCIO::ContextRcPtr& context)
{
    if (!m_config) return nullptr;

    std::ostringstream ctxStr;
    if (context) {
        for (int i = 0; i < context->getNumStringVars(); ++i) {
            auto k = context->getStringVarNameByIndex(i);
            auto v = context->getStringVarByIndex(i);
            ctxStr << k << "=" << v << ";";
        }
    } else {
        ctxStr << "default";
    }

    ProcessorKey key{srcColorSpace, dstColorSpace, ctxStr.str()};
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    try {
        auto proc = m_config->getProcessor(srcColorSpace.c_str(), dstColorSpace.c_str());
        m_cache[key] = proc;
        return proc;
    } catch (const OCIO::Exception& e) {
        m_lastError = e.what();
        std::cerr << "OCIO getProcessor failed: " << e.what() << std::endl;
        return nullptr;
    }
}

OCIO::ConstProcessorRcPtr OCIOWrapper::getDisplayProcessor(
    const std::string& display, const std::string& view,
    const std::string& inputColorSpace)
{
    if (!m_config) return nullptr;

    try {
        auto proc = m_config->getProcessor(inputColorSpace.c_str(),
                                           display.c_str(), view.c_str(),
                                           OCIO::TRANSFORM_DIR_FORWARD);
        return proc;
    } catch (const OCIO::Exception& e) {
        m_lastError = e.what();
        std::cerr << "OCIO getDisplayProcessor failed: " << e.what() << std::endl;
        return nullptr;
    }
}

bool OCIOWrapper::apply(OCIO::ConstProcessorRcPtr processor, ImageBuffer& buffer)
{
    if (!processor) return false;

    try {
        auto cpuProc = processor->getDefaultCPUProcessor();
        int numChan = std::min(buffer.channels(), 4);
        OCIO::PackedImageDesc desc(buffer.data(),
                                   static_cast<long>(buffer.width()),
                                   static_cast<long>(buffer.height()),
                                   numChan);
        cpuProc->apply(desc);
        return true;
    } catch (const OCIO::Exception& e) {
        m_lastError = e.what();
        std::cerr << "OCIO apply failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> OCIOWrapper::colorSpaces() const
{
    std::vector<std::string> result;
    if (!m_config) return result;
    for (int i = 0; i < m_config->getNumColorSpaces(); ++i)
        result.push_back(m_config->getColorSpaceNameByIndex(i));
    return result;
}

std::vector<std::string> OCIOWrapper::displays() const
{
    std::vector<std::string> result;
    if (!m_config) return result;
    for (int i = 0; i < m_config->getNumDisplays(); ++i)
        result.push_back(m_config->getDisplay(i));
    return result;
}

std::vector<std::string> OCIOWrapper::views(const std::string& display) const
{
    std::vector<std::string> result;
    if (!m_config) return result;
    for (int i = 0; i < m_config->getNumViews(display.c_str()); ++i)
        result.push_back(m_config->getView(display.c_str(), i));
    return result;
}

std::vector<std::string> OCIOWrapper::looks() const
{
    std::vector<std::string> result;
    if (!m_config) return result;
    for (int i = 0; i < m_config->getNumLooks(); ++i)
        result.push_back(m_config->getLookNameByIndex(i));
    return result;
}

void OCIOWrapper::clearCache()
{
    m_cache.clear();
}

void OCIOWrapper::setContextKey(const std::string& key, const std::string& value)
{
    if (m_context)
        m_context->setStringVar(key.c_str(), value.c_str());
}

} // namespace mokm
