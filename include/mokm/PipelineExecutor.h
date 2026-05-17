#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <QVariant>

#include "mokm/NodeGraph.h"

namespace mokm {

class ImageBuffer;
class OCIOWrapper;

class PipelineExecutor {
public:
    PipelineExecutor();
    ~PipelineExecutor() = default;

    bool loadGraph(const GraphDesc& graph);
    bool loadGraphFromJson(const std::string& json);

    bool processImage(ImageBuffer& buffer);
    bool processSequence(const std::string& pattern, int start, int end, const std::string& outputPattern);

    GraphDesc graph() const { return m_graph; }
    std::string lastError() const { return m_lastError; }

private:
    struct NodeInstance {
        NodeDesc desc;
        bool dirty = true;
        std::map<std::string, QVariant> cachedOutputs;
    };

    using TopoOrder = std::vector<std::string>;

    TopoOrder topologicalSort() const;
    bool hasCycles() const;
    QVariantMap evaluateNode(const std::string& nodeId, const ImageBuffer* input);
    ImageBuffer* resolveInputBuffer(const std::string& nodeId);

    GraphDesc m_graph;
    std::map<std::string, NodeInstance> m_nodes;
    std::map<std::string, ImageBuffer> m_buffers;
    std::string m_lastError;
};

} // namespace mokm
