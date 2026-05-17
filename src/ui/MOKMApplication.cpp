#include "mokm/MOKMApplication.h"

#include <QApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QWindow>

#include "nodes/registerMOKMNodes.h"
#include "ui/PreviewWindow.h"
#include "nodes/output/PreviewNode.h"

#include <NodeEditor/DefaultNodes.h>
#include <NodeEditor/GraphModel.h>
#include <NodeEditor/DataFlowEngine.h>
#include <NodeEditor/UndoManager.h>

namespace mokm {

class MOKMApplication::Private {
public:
    Private(int &argc, char **argv)
        : app(argc, argv)
    {
        QQuickStyle::setStyle("Basic");
    }

    ~Private() = default;

    int exec() {
        engine->loadFromModule("MOKM_Color_Processor", "SplashScreen");
        return QCoreApplication::exec();
    }

    QApplication app;
    NodeEditor::GraphModel registryModel;
    PreviewWindow *previewWindow = nullptr;
    QQmlApplicationEngine *engine = nullptr;
};

MOKMApplication::MOKMApplication(int &argc, char **argv)
    : d(std::make_unique<Private>(argc, argv))
{
    NodeEditor::registerDefaultNodeTypes(&d->registryModel);
    registerMOKMNodeTypes(&d->registryModel);

    qRegisterMetaType<mokm::ImageBufferPtr>("mokm::ImageBufferPtr");

    d->previewWindow = new PreviewWindow();
    d->previewWindow->hide();

    PreviewNode::setPreviewCallback([this](QImage img, QString cs) {
        QMetaObject::invokeMethod(d->previewWindow, [this, img, cs]() {
            d->previewWindow->showImage(img, cs);
        }, Qt::QueuedConnection);
    });

    d->engine = new QQmlApplicationEngine();

    QString binDir = QCoreApplication::applicationDirPath();
    QStringList candidates = {
        binDir + "/../third_parties/node",
        binDir + "/../lib",
    };
    for (const auto &path : candidates) {
        d->engine->addImportPath(path);
    }

    qmlRegisterType<NodeEditor::GraphModel>("NodeEditor", 1, 0, "GraphModel");
    qmlRegisterType<NodeEditor::DataFlowEngine>("NodeEditor", 1, 0, "DataFlowEngine");
    qmlRegisterType<NodeEditor::UndoManager>("NodeEditor", 1, 0, "UndoManager");

    d->engine->rootContext()->setContextProperty("_nodeTypeRegistry", &d->registryModel);
    d->engine->rootContext()->setContextProperty("_previewWindow", d->previewWindow);

    QObject::connect(
        d->engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &d->app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
}

MOKMApplication::~MOKMApplication() = default;

int MOKMApplication::exec()
{
    return d->exec();
}

QWindow *MOKMApplication::mainWindow() const
{
    if (!d->engine) return nullptr;
    const auto windows = d->engine->rootObjects();
    for (auto *obj : windows) {
        if (auto *w = qobject_cast<QQuickWindow *>(obj))
            return w;
    }
    return nullptr;
}

NodeEditor::GraphModel *MOKMApplication::registryModel() const
{
    return &d->registryModel;
}

PreviewWindow *MOKMApplication::previewWindow() const
{
    return d->previewWindow;
}

QQmlApplicationEngine *MOKMApplication::engine() const
{
    return d->engine;
}

} // namespace mokm
