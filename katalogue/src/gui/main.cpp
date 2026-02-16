#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QStandardPaths>
#include <QTranslator>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QProcess>
#include <QDir>
#include <QThread>

#include "katalogue_client.h"

static void ensureDaemonRunning() {
    auto *bus = QDBusConnection::sessionBus().interface();
    if (bus && bus->isServiceRegistered(QStringLiteral("org.kde.Katalogue1"))) {
        return;
    }

    // Look for katalogued next to the GUI binary
    const QString appDir = QCoreApplication::applicationDirPath();
    QString daemonPath = appDir + QStringLiteral("/katalogued");
    if (!QFile::exists(daemonPath)) {
        // Fallback: search PATH
        daemonPath = QStandardPaths::findExecutable(QStringLiteral("katalogued"));
    }
    if (daemonPath.isEmpty()) {
        qWarning() << "katalogued not found; start it manually.";
        return;
    }

    QProcess::startDetached(daemonPath, {});

    // Wait up to 3 seconds for the service to appear
    for (int i = 0; i < 30; ++i) {
        QThread::msleep(100);
        if (bus && bus->isServiceRegistered(QStringLiteral("org.kde.Katalogue1"))) {
            return;
        }
    }
    qWarning() << "katalogued started but service not yet registered.";
}

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Katalogue"));
    app.setApplicationName(QStringLiteral("Katalogue"));
    app.setApplicationDisplayName(QObject::tr("Katalogue"));

    ensureDaemonRunning();

    QTranslator translator;
    const QString locale = QLocale::system().name();
    const QString translationsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                     + QStringLiteral("/translations");
    if (translator.load(QStringLiteral("katalogue_") + locale, translationsPath)) {
        app.installTranslator(&translator);
    }

    QQmlApplicationEngine engine;
    KatalogueClient client;
    qmlRegisterSingletonInstance("org.kde.Katalogue", 1, 0, "KatalogueClient", &client);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [](QObject *obj) {
                         if (!obj) {
                             QCoreApplication::exit(-1);
                         }
                     }, Qt::QueuedConnection);
    engine.loadFromModule("org.kde.Katalogue", "Main");

    return app.exec();
}
