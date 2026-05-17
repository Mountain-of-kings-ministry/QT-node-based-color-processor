#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

class LookTransformNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/ocio/lookTransform"; }
    QString nodeName() const override { return "Look Transform"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "OCIO"; }
    QString displayColor() const override { return "#FACC15"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::String, "look", QVariant("")},
            {NodeEditor::PortType::String, "srcColorSpace", QVariant("scene_linear")},
            {NodeEditor::PortType::String, "dstColorSpace", QVariant("scene_linear")}
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

        auto lookName = inputs.value("look").toString().toStdString();
        auto srcCS = inputs.value("srcColorSpace").toString().toStdString();
        auto dstCS = inputs.value("dstColorSpace").toString().toStdString();

        auto& ocio = mokm::OCIOWrapper::instance();
        auto config = ocio.config();
        if (!config) return out;

        try {
            auto lookTransform = OCIO::LookTransform::Create();
            lookTransform->setLooks(lookName.c_str());
            lookTransform->setSrc(srcCS.c_str());
            lookTransform->setDst(dstCS.c_str());

            auto proc = config->getProcessor(lookTransform);
            auto result = std::make_shared<mokm::ImageBuffer>(srcBuf->clone());
            ocio.apply(proc, *result);
            out["image"] = QVariant::fromValue(result);
        } catch (const OCIO::Exception& e) {
            qWarning() << "LookTransformNode error:" << e.what();
        }

        return out;
    }
};
