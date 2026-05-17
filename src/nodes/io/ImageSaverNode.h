#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"

class ImageSaverNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/io/imageSaver"; }
    QString nodeName() const override { return "Image Saver"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "IO"; }
    QString displayColor() const override { return "#EF4444"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::String, "path", QVariant("output.ppm")},
            {NodeEditor::PortType::String, "format", QVariant("ppm")}
        };
    }

    QList<NodeEditor::PortInfo> outputSpec() const override {
        return {{NodeEditor::PortType::Image, "image", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantMap out;
        auto imgVar = inputs.value("image");
        auto path = inputs.value("path").toString();
        if (!imgVar.isValid() || path.isEmpty()) return out;

        auto buf = imgVar.value<mokm::ImageBufferPtr>();
        if (!buf) return out;

        buf->saveToFile(path.toStdString());
        out["image"] = imgVar;
        return out;
    }
};
