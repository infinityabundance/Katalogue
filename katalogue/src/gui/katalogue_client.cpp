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

QVariantList KatalogueClient::activeScans() const {
    return m_activeScans;
}

QString KatalogueClient::projectPath() const {
    return m_projectPath;
}

QVariantMap KatalogueClient::projectInfo() const {
    return m_projectInfo;
}

void KatalogueClient::openProject(const QString &path) {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantMap> reply = m_iface->call(QStringLiteral("OpenProject"), path);
    if (!reply.isValid()) {
        return;
    }
    m_projectInfo = reply.value();
    m_projectPath = m_projectInfo.value(QStringLiteral("path")).toString();
    emit projectInfoChanged();
    emit projectPathChanged();

    if (m_projectInfo.value(QStringLiteral("ok")).toBool()) {
        m_directoryEntries.clear();
        m_fileEntries.clear();
        m_searchResults.clear();
        emit directoryEntriesChanged();
        emit fileEntriesChanged();
        emit searchResultsChanged();
        refreshVolumes();
    }
}

int KatalogueClient::startScan(const QString &rootPath) {
    if (!ensureInterface()) {
        return 0;
    }
    QDBusReply<uint> reply = m_iface->call(QStringLiteral("StartScan"), rootPath);
    if (!reply.isValid()) {
        return 0;
    }
    const int scanId = static_cast<int>(reply.value());
    if (scanId <= 0) {
        return 0;
    }

    ScanInfo info;
    info.id = scanId;
    info.rootPath = rootPath;
    info.status = QStringLiteral("running");
    m_scans.insert(scanId, info);
    rebuildActiveScans();
    emit activeScansChanged();

    return scanId;
}

void KatalogueClient::cancelScan(int scanId) {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<bool> reply = m_iface->call(QStringLiteral("CancelScan"), static_cast<uint>(scanId));
    if (reply.isValid() && reply.value()) {
        if (m_scans.contains(scanId)) {
            m_scans[scanId].status = QStringLiteral("cancelled");
            rebuildActiveScans();
            emit activeScansChanged();
        }
    }
}

QVariantMap KatalogueClient::scanStatus(int scanId) const {
    if (!m_iface) {
        return {};
    }
    QDBusReply<QVariantMap> reply = m_iface->call(QStringLiteral("GetScanStatus"),
                                                  static_cast<uint>(scanId));
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
    search(query, -1, QString(), limit, offset);
}

void KatalogueClient::search(const QString &query,
                             int volumeId,
                             const QString &fileType,
                             int limit,
                             int offset) {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantList> reply = m_iface->call(QStringLiteral("Search"),
                                                   query,
                                                   volumeId,
                                                   fileType,
                                                   limit,
                                                   offset);
    if (!reply.isValid()) {
        return;
    }
    m_searchResults = reply.value();
    emit searchResultsChanged();
}

void KatalogueClient::jumpToResult(int volumeId, int directoryId) {
    if (volumeId >= 0) {
        setSelectedVolumeId(volumeId);
    }
    if (directoryId >= 0) {
        setSelectedDirectoryId(directoryId);
    }
}

QString KatalogueClient::getFileNote(int fileId) {
    if (!ensureInterface()) {
        return {};
    }
    QDBusReply<QString> reply = m_iface->call(QStringLiteral("GetFileNote"), fileId);
    return reply.isValid() ? reply.value() : QString();
}

void KatalogueClient::setFileNote(int fileId, const QString &content) {
    if (!ensureInterface()) {
        return;
    }
    m_iface->call(QStringLiteral("SetFileNote"), fileId, content);
}

QVariantList KatalogueClient::getFileTags(int fileId) {
    if (!ensureInterface()) {
        return {};
    }
    QDBusReply<QVariantList> reply = m_iface->call(QStringLiteral("GetFileTags"), fileId);
    return reply.isValid() ? reply.value() : QVariantList{};
}

void KatalogueClient::addFileTag(int fileId, const QString &key, const QString &value) {
    if (!ensureInterface()) {
        return;
    }
    m_iface->call(QStringLiteral("AddFileTag"), fileId, key, value);
}

void KatalogueClient::removeFileTag(int fileId, const QString &key, const QString &value) {
    if (!ensureInterface()) {
        return;
    }
    m_iface->call(QStringLiteral("RemoveFileTag"), fileId, key, value);
}

void KatalogueClient::refreshProjectInfo() {
    if (!ensureInterface()) {
        return;
    }
    QDBusReply<QVariantMap> reply = m_iface->call(QStringLiteral("GetProjectInfo"));
    if (!reply.isValid()) {
        return;
    }
    m_projectInfo = reply.value();
    m_projectPath = m_projectInfo.value(QStringLiteral("path")).toString();
    emit projectInfoChanged();
    emit projectPathChanged();
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

void KatalogueClient::rebuildActiveScans() {
    QVariantList list;
    list.reserve(m_scans.size());
    for (const auto &info : m_scans) {
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), info.id);
        entry.insert(QStringLiteral("rootPath"), info.rootPath);
        entry.insert(QStringLiteral("status"), info.status);
        entry.insert(QStringLiteral("percent"), info.percent);
        entry.insert(QStringLiteral("directories"), static_cast<qulonglong>(info.directories));
        entry.insert(QStringLiteral("files"), static_cast<qulonglong>(info.files));
        entry.insert(QStringLiteral("bytes"), static_cast<qulonglong>(info.bytes));
        entry.insert(QStringLiteral("errorString"), info.errorString);
        list.append(entry);
    }
    m_activeScans = list;
}

void KatalogueClient::onScanProgress(uint scanId,
                                     const QString &path,
                                     int directories,
                                     int files,
                                     qint64 bytes) {
    const int id = static_cast<int>(scanId);
    if (!m_scans.contains(id)) {
        ScanInfo info;
        info.id = id;
        info.rootPath = path;
        info.status = QStringLiteral("running");
        m_scans.insert(id, info);
    }
    auto &info = m_scans[id];
    info.status = QStringLiteral("running");
    info.directories = static_cast<quint64>(directories);
    info.files = static_cast<quint64>(files);
    info.bytes = static_cast<quint64>(bytes);
    rebuildActiveScans();
    emit activeScansChanged();
    emit scanProgress(scanId, path, directories, files, bytes);
}

void KatalogueClient::onScanFinished(uint scanId, const QString &status) {
    const int id = static_cast<int>(scanId);
    if (!m_scans.contains(id)) {
        ScanInfo info;
        info.id = id;
        info.status = status;
        m_scans.insert(id, info);
    } else {
        m_scans[id].status = status;
    }
    rebuildActiveScans();
    emit activeScansChanged();
    emit scanFinished(scanId, status);
    refreshVolumes();
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
        SLOT(onScanProgress(uint,QString,int,int,qint64)));
    QDBusConnection::sessionBus().connect(
        QString::fromUtf8(kService),
        QString::fromUtf8(kPath),
        QString::fromUtf8(kInterface),
        QStringLiteral("ScanFinished"),
        this,
        SLOT(onScanFinished(uint,QString)));
    m_signalsConnected = true;
}
