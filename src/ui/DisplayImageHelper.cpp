#include "DisplayImageHelper.h"
#include "DisplayImageProvider.h"

DisplayImageHelper::DisplayImageHelper(DisplayImageProvider *provider, QObject *parent)
    : QObject(parent)
    , m_provider(provider)
{
}

void DisplayImageHelper::updateImage(const QImage &img, const QString &colorSpace)
{
    m_provider->setImage(img);
    m_counter++;
    m_refreshToken = QString::number(m_counter);
    m_info = QString("%1x%2 | %3").arg(img.width()).arg(img.height()).arg(colorSpace);
    emit imageChanged();
}
