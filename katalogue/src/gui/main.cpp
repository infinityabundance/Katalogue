#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QStandardPaths>
#include <QTranslator>

#include "katalogue_client.h"

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Katalogue"));
    app.setApplicationName(QStringLiteral("Katalogue"));
    app.setApplicationDisplayName(QObject::tr("Katalogue"));

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
    const QUrl url(QStringLiteral("qrc:/org/kde/Katalogue/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) {
                             QCoreApplication::exit(-1);
                         }
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
