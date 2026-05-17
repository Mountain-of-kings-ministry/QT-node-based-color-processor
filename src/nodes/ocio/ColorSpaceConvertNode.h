#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

class ColorSpaceConvertNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/ocio/colorSpaceConvert"; }
    QString nodeName() const override { return "ColorSpace Convert"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "OCIO"; }
    QString displayColor() const override { return "#FACC15"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::String, "srcColorSpace", QVariant("scene_linear")},
            {NodeEditor::PortType::String, "dstColorSpace", QVariant("output_srgb")}
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

        auto srcCS = inputs.value("srcColorSpace").toString().toStdString();
        auto dstCS = inputs.value("dstColorSpace").toString().toStdString();

        auto& ocio = mokm::OCIOWrapper::instance();
        auto proc = ocio.getProcessor(srcCS, dstCS);
        if (!proc) return out;

        auto result = std::make_shared<mokm::ImageBuffer>(srcBuf->clone());
        ocio.apply(proc, *result);
        result->setColorSpace(dstCS);

        out["image"] = QVariant::fromValue(result);
        return out;
    }
};
