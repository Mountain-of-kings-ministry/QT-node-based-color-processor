#include "mokm/PipelineExecutor.h"
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>

namespace mokm {

PipelineExecutor::PipelineExecutor()
{
}

bool PipelineExecutor::loadGraph(const GraphDesc& graph)
{
    if (!graph.validate()) {
        m_lastError = "Invalid graph description";
        return false;
    }

    m_graph = graph;
    m_nodes.clear();
    m_buffers.clear();

    for (const auto& node : graph.nodes) {
        NodeInstance inst;
        inst.desc = node;
        inst.dirty = true;
        m_nodes[node.id] = inst;
    }

    return true;
}

bool PipelineExecutor::loadGraphFromJson(const std::string& json)
{
    auto graph = GraphDesc::deserializeFromJson(json);
    return loadGraph(graph);
}

bool PipelineExecutor::hasCycles() const
{
    std::set<std::string> white, gray, black;
    for (const auto& [id, _] : m_nodes)
        white.insert(id);

    std::function<bool(const std::string&)> dfs = [&](const std::string& u) -> bool {
        white.erase(u);
        gray.insert(u);

        for (const auto& e : m_graph.edges) {
            if (e.sourceNode != u) continue;
            const auto& v = e.targetNode;
            if (gray.count(v)) return true;
            if (white.count(v) && dfs(v)) return true;
        }

        gray.erase(u);
        black.insert(u);
        return false;
    };

    while (!white.empty()) {
        std::string start = *white.begin();
        if (dfs(start)) return true;
    }
    return false;
}

PipelineExecutor::TopoOrder PipelineExecutor::topologicalSort() const
{
    TopoOrder result;
    std::map<std::string, int> inDegree;
    std::map<std::string, std::vector<std::string>> adj;

    for (const auto& [id, _] : m_nodes) {
        inDegree[id] = 0;
        adj[id] = {};
    }

    for (const auto& e : m_graph.edges) {
        adj[e.sourceNode].push_back(e.targetNode);
        inDegree[e.targetNode]++;
    }

    std::queue<std::string> q;
    for (const auto& [id, deg] : inDegree) {
        if (deg == 0) q.push(id);
    }

    while (!q.empty()) {
        auto u = q.front(); q.pop();
        result.push_back(u);
        for (const auto& v : adj[u]) {
            if (--inDegree[v] == 0)
                q.push(v);
        }
    }

    return result;
}

QVariantMap PipelineExecutor::evaluateNode(const std::string& nodeId, const ImageBuffer* input)
{
    QVariantMap result;
    auto it = m_nodes.find(nodeId);
    if (it == m_nodes.end()) return result;

    const auto& desc = it->second.desc;

    if (desc.type == "mokm/io/imageLoader") {
        auto pathIt = desc.params.find("path");
        if (pathIt != desc.params.end()) {
            auto buf = std::make_shared<ImageBuffer>();
            QString path = pathIt->second.toString();
            if (buf->loadFromFile(path.toStdString())) {
                QString cs = "scene_linear";
                auto csIt = desc.params.find("colorSpace");
                if (csIt != desc.params.end())
                    cs = csIt->second.toString();
                buf->setColorSpace(cs.toStdString());
                result["image"] = QVariant::fromValue(buf);
            }
        }
    } else if (desc.type == "mokm/io/imageSaver" && input) {
        auto pathIt = desc.params.find("path");
        if (pathIt != desc.params.end()) {
            QString path = pathIt->second.toString();
            const_cast<ImageBuffer*>(input)->saveToFile(path.toStdString());
        }
        result["image"] = QVariant::fromValue(std::make_shared<ImageBuffer>(input->clone()));
    } else if (desc.type == "mokm/ocio/colorSpaceConvert" && input) {
        auto srcIt = desc.params.find("srcColorSpace");
        auto dstIt = desc.params.find("dstColorSpace");
        if (srcIt != desc.params.end() && dstIt != desc.params.end()) {
            auto& ocio = OCIOWrapper::instance();
            auto proc = ocio.getProcessor(
                srcIt->second.toString().toStdString(),
                dstIt->second.toString().toStdString());
            if (proc) {
                auto buf = std::make_shared<ImageBuffer>(input->clone());
                ocio.apply(proc, *buf);
                buf->setColorSpace(dstIt->second.toString().toStdString());
                result["image"] = QVariant::fromValue(buf);
            }
        }
    } else if (desc.type == "mokm/ocio/displayView" && input) {
        auto displayIt = desc.params.find("display");
        auto viewIt = desc.params.find("view");
        if (displayIt != desc.params.end() && viewIt != desc.params.end()) {
            auto& ocio = OCIOWrapper::instance();
            auto proc = ocio.getDisplayProcessor(
                displayIt->second.toString().toStdString(),
                viewIt->second.toString().toStdString(),
                input->colorSpace().c_str());
            if (proc) {
                auto buf = std::make_shared<ImageBuffer>(input->clone());
                ocio.apply(proc, *buf);
                result["image"] = QVariant::fromValue(buf);
            }
        }
    }

    return result;
}

ImageBuffer* PipelineExecutor::resolveInputBuffer(const std::string& nodeId)
{
    for (const auto& e : m_graph.edges) {
        if (e.targetNode == nodeId) {
            auto srcIt = m_buffers.find(e.sourceNode);
            if (srcIt != m_buffers.end())
                return &srcIt->second;
        }
    }
    return nullptr;
}

bool PipelineExecutor::processImage(ImageBuffer& buffer)
{
    if (hasCycles()) {
        m_lastError = "Graph contains cycles";
        return false;
    }

    auto order = topologicalSort();
    if (order.empty()) {
        m_lastError = "Empty graph or topological sort failed";
        return false;
    }

    auto& ocio = OCIOWrapper::instance();
    if (!m_graph.ocioConfigPath.empty() && !ocio.isLoaded())
        ocio.loadConfig(m_graph.ocioConfigPath);

    for (const auto& nodeId : order) {
        auto it = m_nodes.find(nodeId);
        if (it == m_nodes.end()) continue;

        ImageBuffer* input = resolveInputBuffer(nodeId);
        if (!input) input = &buffer;

        auto outputs = evaluateNode(nodeId, input);

        auto imgVar = outputs.value("image");
        if (imgVar.canConvert<ImageBufferPtr>()) {
            auto ptr = imgVar.value<ImageBufferPtr>();
            if (ptr)
                m_buffers[nodeId] = std::move(*ptr);
        }
        it->second.dirty = false;
    }

    return true;
}

bool PipelineExecutor::processSequence(const std::string& pattern, int start, int end,
                                        const std::string& outputPattern)
{
    for (int frame = start; frame <= end; ++frame) {
        char inputPath[1024], outputPath[1024];
        std::snprintf(inputPath, sizeof(inputPath), pattern.c_str(), frame);
        std::snprintf(outputPath, sizeof(outputPath), outputPattern.c_str(), frame);

        ImageBuffer buf;
        if (!buf.loadFromFile(inputPath)) {
            std::cerr << "Failed to load frame: " << inputPath << std::endl;
            continue;
        }

        if (!processImage(buf)) {
            std::cerr << "Failed to process frame: " << inputPath << std::endl;
            continue;
        }

        if (!buf.saveToFile(outputPath)) {
            std::cerr << "Failed to save frame: " << outputPath << std::endl;
        }
    }
    return true;
}

} // namespace mokm
