#include "katalogue_client.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariantMap>

namespace {
constexpr const char *kService = "org.kde.Katalogue1";
constexpr const char *kPath = "/org/kde/Katalogue1";
constexpr const char *kInterface = "org.kde.Katalogue1";
} // namespace

KatalogueClient::KatalogueClient(QObject *parent)
    : QObject(parent) {}

QVariantList KatalogueClient::volumes() const {
    return m_volumes;
}

QVariantList KatalogueClient::searchResults() const {
    return m_searchResults;
}

bool KatalogueClient::openProject(const QString &path) {
    if (!ensureInterface()) {
        return false;
    }
    QDBusReply<bool> reply = m_iface->call(QStringLiteral("OpenProject"), path);
    return reply.isValid() && reply.value();
}

uint KatalogueClient::startScan(const QString &rootPath) {
    if (!ensureInterface()) {
        return 0;
    }
    QDBusReply<uint> reply = m_iface->call(QStringLiteral("StartScan"), rootPath);
    return reply.isValid() ? reply.value() : 0;
}

bool KatalogueClient::cancelScan(uint scanId) {
    if (!ensureInterface()) {
        return false;
    }
    QDBusReply<bool> reply = m_iface->call(QStringLiteral("CancelScan"), scanId);
    return reply.isValid() && reply.value();
}

QVariantMap KatalogueClient::scanStatus(uint scanId) const {
    if (!m_iface) {
        return {};
    }
    QDBusReply<QVariantMap> reply = m_iface->call(QStringLiteral("GetScanStatus"), scanId);
    return reply.isValid() ? reply.value() : QVariantMap{};
}

void KatalogueClient::refreshVolumes() {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantMap> reply = m_iface->call(QStringLiteral("ListVolumes"));
    if (!reply.isValid()) {
        return;
    }
    const QVariantMap payload = reply.value();
    m_volumes = payload.value(QStringLiteral("items")).toList();
    emit volumesChanged();
}

void KatalogueClient::searchByName(const QString &query, int limit, int offset) {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantMap> reply = m_iface->call(QStringLiteral("SearchByName"), query, limit, offset);
    if (!reply.isValid()) {
        return;
    }
    const QVariantMap payload = reply.value();
    m_searchResults = payload.value(QStringLiteral("items")).toList();
    emit searchResultsChanged();
}

bool KatalogueClient::ensureInterface() {
    if (!m_iface) {
        m_iface = std::make_unique<QDBusInterface>(QString::fromUtf8(kService),
                                                   QString::fromUtf8(kPath),
                                                   QString::fromUtf8(kInterface),
                                                   QDBusConnection::sessionBus());
    }
    if (!m_iface || !m_iface->isValid()) {
        return false;
    }
    if (!m_signalsConnected) {
        connectSignals();
    }
    return true;
}

void KatalogueClient::connectSignals() {
    if (!m_iface) {
        return;
    }
    QDBusConnection::sessionBus().connect(
        QString::fromUtf8(kService),
        QString::fromUtf8(kPath),
        QString::fromUtf8(kInterface),
        QStringLiteral("ScanProgress"),
        this,
        SIGNAL(scanProgress(uint,QString,int,int,qint64)));
    QDBusConnection::sessionBus().connect(
        QString::fromUtf8(kService),
        QString::fromUtf8(kPath),
        QString::fromUtf8(kInterface),
        QStringLiteral("ScanFinished"),
        this,
        SIGNAL(scanFinished(uint,QString)));
    m_signalsConnected = true;
}
