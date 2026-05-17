#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

class FileTransformNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/ocio/fileTransform"; }
    QString nodeName() const override { return "File Transform (LUT)"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "OCIO"; }
    QString displayColor() const override { return "#FACC15"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::String, "lutPath", QVariant("")},
            {NodeEditor::PortType::String, "interpolation", QVariant("linear")}
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

        auto lutPath = inputs.value("lutPath").toString().toStdString();
        if (lutPath.empty()) return out;

        auto& ocio = mokm::OCIOWrapper::instance();
        auto config = ocio.config();
        if (!config) return out;

        try {
            auto fileTransform = OCIO::FileTransform::Create();
            fileTransform->setSrc(lutPath.c_str());

            auto proc = config->getProcessor(fileTransform);
            auto result = std::make_shared<mokm::ImageBuffer>(srcBuf->clone());
            ocio.apply(proc, *result);
            out["image"] = QVariant::fromValue(result);
        } catch (const OCIO::Exception& e) {
            qWarning() << "FileTransformNode error:" << e.what();
        }

        return out;
    }
};
