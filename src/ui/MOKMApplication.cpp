#include "mokm/MOKMApplication.h"

#include <QApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QWindow>

#include "nodes/registerMOKMNodes.h"
#include "ui/DisplayImageProvider.h"
#include "ui/DisplayImageHelper.h"
#include "ui/FileDialogHelper.h"
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
    DisplayImageProvider *displayProvider = nullptr;
    DisplayImageHelper *displayHelper = nullptr;
    QQmlApplicationEngine *engine = nullptr;
};

MOKMApplication::MOKMApplication(int &argc, char **argv)
    : d(std::make_unique<Private>(argc, argv))
{
    NodeEditor::registerDefaultNodeTypes(&d->registryModel);
    registerMOKMNodeTypes(&d->registryModel);

    qRegisterMetaType<mokm::ImageBufferPtr>("mokm::ImageBufferPtr");

    d->displayProvider = new DisplayImageProvider();

    d->displayHelper = new DisplayImageHelper(d->displayProvider);

    PreviewNode::setPreviewCallback([this](QImage img, QString cs) {
        QMetaObject::invokeMethod(d->displayHelper, [this, img, cs]() {
            d->displayHelper->updateImage(img, cs);
        }, Qt::QueuedConnection);
    });

    d->engine = new QQmlApplicationEngine();

    d->engine->addImageProvider(QStringLiteral("mokmpreview"), d->displayProvider);

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

    auto *fileHelper = new FileDialogHelper(d->engine);
    d->engine->rootContext()->setContextProperty("_fileDialog", fileHelper);

    d->engine->rootContext()->setContextProperty("_nodeTypeRegistry", &d->registryModel);
    d->engine->rootContext()->setContextProperty("_displayHelper", d->displayHelper);

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

QQmlApplicationEngine *MOKMApplication::engine() const
{
    return d->engine;
}

} // namespace mokm
