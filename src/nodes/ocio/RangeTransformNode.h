#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

class RangeTransformNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/ocio/rangeTransform"; }
    QString nodeName() const override { return "Range Transform"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "OCIO"; }
    QString displayColor() const override { return "#FACC15"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::Float, "minIn", QVariant(0.0)},
            {NodeEditor::PortType::Float, "maxIn", QVariant(1.0)},
            {NodeEditor::PortType::Float, "minOut", QVariant(0.0)},
            {NodeEditor::PortType::Float, "maxOut", QVariant(1.0)}
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

        double minIn = inputs.value("minIn").toDouble();
        double maxIn = inputs.value("maxIn").toDouble();
        double minOut = inputs.value("minOut").toDouble();
        double maxOut = inputs.value("maxOut").toDouble();

        auto& ocio = mokm::OCIOWrapper::instance();
        auto config = ocio.config();
        if (!config) return out;

        try {
            auto range = OCIO::RangeTransform::Create();
            range->setMinInValue(minIn);
            range->setMaxInValue(maxIn);
            range->setMinOutValue(minOut);
            range->setMaxOutValue(maxOut);

            auto proc = config->getProcessor(range);
            auto result = std::make_shared<mokm::ImageBuffer>(srcBuf->clone());
            ocio.apply(proc, *result);
            out["image"] = QVariant::fromValue(result);
        } catch (const OCIO::Exception& e) {
            qWarning() << "RangeTransformNode error:" << e.what();
        }

        return out;
    }
};
