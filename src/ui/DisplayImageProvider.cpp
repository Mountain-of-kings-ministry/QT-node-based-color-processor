#include "DisplayImageProvider.h"

DisplayImageProvider::DisplayImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage DisplayImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QMutexLocker lock(&m_mutex);
    Q_UNUSED(id)
    if (size)
        *size = m_image.size();
    if (requestedSize.isValid() && !m_image.isNull())
        return m_image.scaled(requestedSize);
    return m_image;
}

void DisplayImageProvider::setImage(const QImage &img)
{
    QMutexLocker lock(&m_mutex);
    m_image = img;
}
