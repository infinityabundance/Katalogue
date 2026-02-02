#include "katalogue_daemon.h"

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QDBusConnection>
#include <QDBusError>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    KatalogueDaemon daemon;
    if (!daemon.init()) {
        qCritical() << QObject::tr("Katalogue daemon initialization failed.");
        return 1;
    }

    if (!QDBusConnection::sessionBus().isConnected()) {
        qCritical() << QObject::tr("Failed to connect to session bus")
                    << QDBusConnection::sessionBus().lastError().message();
        return 1;
    }

    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.Katalogue1"))) {
        qCritical() << QObject::tr("Failed to register service")
                    << QDBusConnection::sessionBus().lastError().message();
        return 1;
    }

    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/kde/Katalogue1"),
                                                      &daemon,
                                                      QDBusConnection::ExportAllSlots |
                                                          QDBusConnection::ExportAllSignals)) {
        qCritical() << QObject::tr("Failed to register object")
                    << QDBusConnection::sessionBus().lastError().message();
        return 1;
    }

    qInfo() << QObject::tr("Katalogue daemon started (DBus)");

    return app.exec();
}
