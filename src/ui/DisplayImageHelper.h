#pragma once

#include <QObject>
#include <QImage>
#include <QString>

class DisplayImageProvider;

class DisplayImageHelper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString refreshToken READ refreshToken NOTIFY imageChanged)
    Q_PROPERTY(QString info READ info NOTIFY imageChanged)
public:
    explicit DisplayImageHelper(DisplayImageProvider *provider, QObject *parent = nullptr);

    QString refreshToken() const { return m_refreshToken; }
    QString info() const { return m_info; }

    Q_INVOKABLE void updateImage(const QImage &img, const QString &colorSpace);

signals:
    void imageChanged();

private:
    DisplayImageProvider *m_provider;
    QString m_refreshToken;
    QString m_info;
    int m_counter = 0;
};
