#include <QApplication>

#include "mokm/MOKMApplication.h"

int main(int argc, char *argv[])
{
    mokm::MOKMApplication app(argc, argv);
    return app.exec();
}
