#include "mokm/NodeGraph.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <set>

namespace mokm {

bool GraphDesc::validate() const
{
    if (nodes.empty()) return false;

    std::set<std::string> nodeIds;
    for (const auto& n : nodes) {
        if (n.id.empty() || n.type.empty()) return false;
        if (!nodeIds.insert(n.id).second) return false;
    }

    for (const auto& e : edges) {
        if (nodeIds.find(e.sourceNode) == nodeIds.end()) return false;
        if (nodeIds.find(e.targetNode) == nodeIds.end()) return false;
    }

    return true;
}

std::string GraphDesc::serializeToJson() const
{
    QJsonObject root;
    root["ocioConfig"] = QString::fromStdString(ocioConfigPath);

    QJsonArray nodesArr;
    for (const auto& n : nodes) {
        QJsonObject obj;
        obj["id"] = QString::fromStdString(n.id);
        obj["type"] = QString::fromStdString(n.type);
        obj["x"] = n.x;
        obj["y"] = n.y;
        QJsonObject params;
        for (const auto& [k, v] : n.params)
            params[QString::fromStdString(k)] = QJsonValue::fromVariant(v);
        obj["params"] = params;
        nodesArr.append(obj);
    }
    root["nodes"] = nodesArr;

    QJsonArray edgesArr;
    for (const auto& e : edges) {
        QJsonObject obj;
        obj["id"] = QString::fromStdString(e.id);
        obj["sourceNode"] = QString::fromStdString(e.sourceNode);
        obj["sourcePort"] = QString::fromStdString(e.sourcePort);
        obj["targetNode"] = QString::fromStdString(e.targetNode);
        obj["targetPort"] = QString::fromStdString(e.targetPort);
        edgesArr.append(obj);
    }
    root["edges"] = edgesArr;

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented).toStdString();
}

GraphDesc GraphDesc::deserializeFromJson(const std::string& json)
{
    GraphDesc desc;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (doc.isNull() || !doc.isObject()) return desc;

    QJsonObject root = doc.object();
    desc.ocioConfigPath = root["ocioConfig"].toString().toStdString();

    QJsonArray nodesArr = root["nodes"].toArray();
    for (const auto& val : nodesArr) {
        QJsonObject obj = val.toObject();
        NodeDesc n;
        n.id = obj["id"].toString().toStdString();
        n.type = obj["type"].toString().toStdString();
        n.x = obj["x"].toDouble();
        n.y = obj["y"].toDouble();
        QJsonObject params = obj["params"].toObject();
        for (auto it = params.begin(); it != params.end(); ++it)
            n.params[it.key().toStdString()] = it.value().toVariant();
        desc.nodes.push_back(n);
    }

    QJsonArray edgesArr = root["edges"].toArray();
    for (const auto& val : edgesArr) {
        QJsonObject obj = val.toObject();
        EdgeDesc e;
        e.id = obj["id"].toString().toStdString();
        e.sourceNode = obj["sourceNode"].toString().toStdString();
        e.sourcePort = obj["sourcePort"].toString().toStdString();
        e.targetNode = obj["targetNode"].toString().toStdString();
        e.targetPort = obj["targetPort"].toString().toStdString();
        desc.edges.push_back(e);
    }

    return desc;
}

} // namespace mokm
