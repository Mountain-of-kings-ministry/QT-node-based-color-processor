#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <QVariant>

namespace mokm {

struct NodePort {
    std::string name;
    std::string type;
    QVariant defaultValue;
};

struct NodeDesc {
    std::string id;
    std::string type;
    double x = 0, y = 0;
    std::map<std::string, QVariant> params;
};

struct EdgeDesc {
    std::string id;
    std::string sourceNode;
    std::string sourcePort;
    std::string targetNode;
    std::string targetPort;
};

struct GraphDesc {
    std::string ocioConfigPath;
    std::vector<NodeDesc> nodes;
    std::vector<EdgeDesc> edges;

    bool validate() const;
    std::string serializeToJson() const;
    static GraphDesc deserializeFromJson(const std::string& json);
};

} // namespace mokm
