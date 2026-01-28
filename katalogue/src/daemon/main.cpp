#include "katalogue_daemon.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    KatalogueDaemon daemon;
    qInfo() << "Katalogue daemon started";

    return app.exec();
}
