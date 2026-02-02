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

int KatalogueClient::selectedVolumeId() const {
    return m_selectedVolumeId;
}

void KatalogueClient::setSelectedVolumeId(int volumeId) {
    if (m_selectedVolumeId == volumeId) {
        return;
    }
    m_selectedVolumeId = volumeId;
    emit selectedVolumeIdChanged();
    if (m_selectedVolumeId >= 0) {
        loadRootForVolume(m_selectedVolumeId);
    }
}

int KatalogueClient::selectedDirectoryId() const {
    return m_selectedDirectoryId;
}

void KatalogueClient::setSelectedDirectoryId(int directoryId) {
    if (m_selectedDirectoryId == directoryId) {
        return;
    }
    m_selectedDirectoryId = directoryId;
    emit selectedDirectoryIdChanged();
    if (m_selectedDirectoryId >= 0) {
        loadDirectory(m_selectedDirectoryId);
    }
}

QVariantList KatalogueClient::directoryEntries() const {
    return m_directoryEntries;
}

QVariantList KatalogueClient::fileEntries() const {
    return m_fileEntries;
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

    if (m_selectedVolumeId < 0 && !m_volumes.isEmpty()) {
        const QVariantMap first = m_volumes.first().toMap();
        if (first.contains(QStringLiteral("id"))) {
            setSelectedVolumeId(first.value(QStringLiteral("id")).toInt());
        }
    }
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

void KatalogueClient::loadRootForVolume(int volumeId) {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantList> reply = m_iface->call(QStringLiteral("ListDirectories"),
                                                   volumeId,
                                                   -1);
    if (!reply.isValid()) {
        return;
    }
    m_directoryEntries = reply.value();
    emit directoryEntriesChanged();
    if (!m_fileEntries.isEmpty()) {
        m_fileEntries.clear();
        emit fileEntriesChanged();
    }
}

void KatalogueClient::loadDirectory(int directoryId) {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantList> dirReply = m_iface->call(QStringLiteral("ListDirectories"),
                                                      m_selectedVolumeId,
                                                      directoryId);
    if (dirReply.isValid()) {
        m_directoryEntries = dirReply.value();
        emit directoryEntriesChanged();
    }

    QDBusReply<QVariantList> fileReply = m_iface->call(QStringLiteral("ListFiles"),
                                                       directoryId);
    if (fileReply.isValid()) {
        m_fileEntries = fileReply.value();
        emit fileEntriesChanged();
    }
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
