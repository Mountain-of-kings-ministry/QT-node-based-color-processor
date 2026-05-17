#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

class DisplayViewNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/ocio/displayView"; }
    QString nodeName() const override { return "Display + View"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "OCIO"; }
    QString displayColor() const override { return "#FACC15"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::String, "display", QVariant("sRGB")},
            {NodeEditor::PortType::String, "view", QVariant("Default")},
            {NodeEditor::PortType::String, "inputColorSpace", QVariant("scene_linear")}
        };
    }

    QList<NodeEditor::PortInfo> outputSpec() const override {
        return {{NodeEditor::PortType::Image, "image", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantMap out;
        auto imgVar = inputs.value("image");
        if (!imgVar.isValid()) return out;

        auto srcBuf = imgVar.value<mokm::ImageBufferPtr>();
        if (!srcBuf) return out;

        auto display = inputs.value("display").toString().toStdString();
        auto view = inputs.value("view").toString().toStdString();
        auto inputCS = inputs.value("inputColorSpace").toString().toStdString();

        auto& ocio = mokm::OCIOWrapper::instance();
        auto proc = ocio.getDisplayProcessor(display, view, inputCS);
        if (!proc) return out;

        auto result = std::make_shared<mokm::ImageBuffer>(srcBuf->clone());
        ocio.apply(proc, *result);

        out["image"] = QVariant::fromValue(result);
        return out;
    }
};
