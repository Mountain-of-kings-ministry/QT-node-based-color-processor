#pragma once

#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

class DisplayImageProvider : public QQuickImageProvider {
public:
    DisplayImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    void setImage(const QImage &img);

private:
    QImage m_image;
    QMutex m_mutex;
};
