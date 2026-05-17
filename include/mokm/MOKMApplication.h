#pragma once

#include <memory>

#include "mokm/Export.h"

class QQmlApplicationEngine;
class QWindow;

namespace NodeEditor { class GraphModel; }

class PreviewWindow;

namespace mokm {

class MOKM_EXPORT MOKMApplication {
public:
    explicit MOKMApplication(int &argc, char **argv);
    ~MOKMApplication();

    int exec();
    QWindow *mainWindow() const;
    NodeEditor::GraphModel *registryModel() const;
    PreviewWindow *previewWindow() const;
    QQmlApplicationEngine *engine() const;

private:
    class Private;
    std::unique_ptr<Private> d;
};

} // namespace mokm
