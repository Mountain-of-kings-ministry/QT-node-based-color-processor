#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"

class ImageLoaderNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/io/imageLoader"; }
    QString nodeName() const override { return "Image Loader"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "IO"; }
    QString displayColor() const override { return "#22C55E"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::String, "path", QVariant("")},
            {NodeEditor::PortType::String, "colorSpace", QVariant("scene_linear")}
        };
    }

    QList<NodeEditor::PortInfo> outputSpec() const override {
        return {{NodeEditor::PortType::Image, "image", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantMap out;
        auto path = inputs.value("path").toString();
        if (path.isEmpty()) return out;

        auto buf = std::make_shared<mokm::ImageBuffer>();
        if (buf->loadFromFile(path.toStdString())) {
            auto cs = inputs.value("colorSpace").toString();
            buf->setColorSpace(cs.toStdString());
            out["image"] = QVariant::fromValue(buf);
        }

        return out;
    }
};
