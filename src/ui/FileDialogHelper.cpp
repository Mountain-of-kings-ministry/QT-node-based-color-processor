#include "FileDialogHelper.h"

#include <QFileDialog>

FileDialogHelper::FileDialogHelper(QObject *parent)
    : QObject(parent)
{
}

QString FileDialogHelper::openFile(const QString &title, const QString &filter)
{
    return QFileDialog::getOpenFileName(nullptr, title, {}, filter);
}

QString FileDialogHelper::saveFile(const QString &title, const QString &filter,
                                    const QString &defaultName)
{
    return QFileDialog::getSaveFileName(nullptr, title, defaultName, filter);
}

QString FileDialogHelper::openDirectory(const QString &title)
{
    return QFileDialog::getExistingDirectory(nullptr, title);
}
