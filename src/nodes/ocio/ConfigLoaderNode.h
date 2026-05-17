#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/OCIOWrapper.h"

class ConfigLoaderNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/ocio/configLoader"; }
    QString nodeName() const override { return "OCIO Config"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "OCIO"; }
    QString displayColor() const override { return "#FACC15"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {{NodeEditor::PortType::String, "configPath", QVariant("")}};
    }

    QList<NodeEditor::PortInfo> outputSpec() const override {
        return {{NodeEditor::PortType::String, "configRef", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        auto path = inputs.value("configPath").toString();
        if (!path.isEmpty()) {
            auto& ocio = mokm::OCIOWrapper::instance();
            ocio.loadConfig(path.toStdString());
        }
        QVariantMap out;
        out["configRef"] = path;
        return out;
    }
};
