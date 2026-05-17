#pragma once

#include <QObject>
#include <QString>

class FileDialogHelper : public QObject {
    Q_OBJECT
public:
    explicit FileDialogHelper(QObject *parent = nullptr);

    Q_INVOKABLE QString openFile(const QString &title = {},
                                 const QString &filter = {});
    Q_INVOKABLE QString saveFile(const QString &title = {},
                                 const QString &filter = {},
                                 const QString &defaultName = {});
    Q_INVOKABLE QString openDirectory(const QString &title = {});
};
