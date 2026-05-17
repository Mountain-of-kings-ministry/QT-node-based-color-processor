#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"

#include <QObject>
#include <QImage>
#include <functional>

class PreviewNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    using PreviewCallback = std::function<void(QImage, QString)>;

    static void setPreviewCallback(PreviewCallback cb) { s_callback = cb; }

    QString nodeType() const override { return "mokm/output/preview"; }
    QString nodeName() const override { return "Preview"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "Output"; }
    QString displayColor() const override { return "#3B82F6"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {{NodeEditor::PortType::Image, "image", QVariant()}};
    }

    QList<NodeEditor::PortInfo> outputSpec() const override {
        return {{NodeEditor::PortType::Image, "image", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantMap out;
        auto imgVar = inputs.value("image");
        if (!imgVar.isValid()) return out;

        auto buf = imgVar.value<mokm::ImageBufferPtr>();
        if (!buf) return out;

        QImage img(buf->width(), buf->height(), QImage::Format_ARGB32);
        for (int y = 0; y < buf->height(); ++y) {
            for (int x = 0; x < buf->width(); ++x) {
                size_t idx = static_cast<size_t>(y) * buf->width() * buf->channels()
                           + static_cast<size_t>(x) * buf->channels();
                float r = buf->data()[idx];
                float g = buf->data()[idx + 1];
                float b = buf->data()[idx + 2];
                float a = buf->channels() > 3 ? buf->data()[idx + 3] : 1.0f;
                img.setPixelColor(x, y, QColor::fromRgbF(
                    std::clamp(r, 0.0f, 1.0f),
                    std::clamp(g, 0.0f, 1.0f),
                    std::clamp(b, 0.0f, 1.0f),
                    std::clamp(a, 0.0f, 1.0f)));
            }
        }

        if (s_callback)
            s_callback(img, QString::fromStdString(buf->colorSpace()));

        emit previewReady(img, QString::fromStdString(buf->colorSpace()));
        out["image"] = imgVar;
        return out;
    }

signals:
    void previewReady(QImage image, QString colorSpace);

private:
    static PreviewCallback s_callback;
};
