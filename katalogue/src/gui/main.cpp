#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "katalogue_client.h"

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);

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
