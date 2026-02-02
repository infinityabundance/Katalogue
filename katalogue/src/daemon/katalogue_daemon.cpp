#include "katalogue_daemon.h"

#include <QDateTime>
#include <QDir>
#include <QDBusError>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStorageInfo>

namespace {
QString defaultProjectPath() {
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(base).filePath(QStringLiteral("catalog.kdcatalog"));
}
} // namespace

KatalogueDaemon::KatalogueDaemon(QObject *parent)
    : QObject(parent) {
    m_projectPath = defaultProjectPath();
    m_scanThread.setObjectName(QStringLiteral("katalogue-scan-thread"));
}

bool KatalogueDaemon::init() {
    if (m_projectPath.isEmpty()) {
        qCritical() << "Default project path is empty";
        return false;
    }
    QDir().mkpath(QFileInfo(m_projectPath).absolutePath());
    if (!m_db.openProject(m_projectPath)) {
        qCritical() << "Failed to open default project:" << m_db.lastErrorString();
        return false;
    }
    if (m_db.checkSchema() != KatalogueDatabase::SchemaStatus::Ok) {
        qCritical() << "Catalog schema problem:" << m_db.lastErrorString();
        return false;
    }
    return true;
}

QString KatalogueDaemon::Ping() const {
    return QStringLiteral("pong");
}

QVariantMap KatalogueDaemon::OpenProject(const QString &path) {
    QVariantMap info;
    const QString cleaned = QDir::cleanPath(path);
    if (cleaned.trimmed().isEmpty()) {
        info.insert(QStringLiteral("ok"), false);
        info.insert(QStringLiteral("path"), QString());
        info.insert(QStringLiteral("volumeCount"), 0);
        info.insert(QStringLiteral("fileCount"), 0);
        info.insert(QStringLiteral("totalBytes"), static_cast<qint64>(0));
        return info;
    }

    const QFileInfo fileInfo(cleaned);
    QDir().mkpath(fileInfo.absolutePath());
    const QString absPath = fileInfo.absoluteFilePath();

    const bool ok = m_db.openProject(absPath);
    if (ok) {
        m_projectPath = absPath;
    } else if (calledFromDBus()) {
        sendErrorReply(QDBusError::Failed,
                       QStringLiteral("Failed to open catalog: %1").arg(m_db.lastErrorString()));
    }

    info.insert(QStringLiteral("ok"), ok);
    info.insert(QStringLiteral("path"), absPath);

    const auto stats = m_db.projectStats();
    if (stats.has_value()) {
        info.insert(QStringLiteral("volumeCount"), stats->volumeCount);
        info.insert(QStringLiteral("fileCount"), static_cast<qint64>(stats->fileCount));
        info.insert(QStringLiteral("totalBytes"), static_cast<qint64>(stats->totalBytes));
    } else {
        info.insert(QStringLiteral("volumeCount"), 0);
        info.insert(QStringLiteral("fileCount"), static_cast<qint64>(0));
        info.insert(QStringLiteral("totalBytes"), static_cast<qint64>(0));
    }

    return info;
}

QVariantMap KatalogueDaemon::GetProjectInfo() const {
    QVariantMap info;
    if (m_projectPath.isEmpty() || !m_db.isOpen()) {
        info.insert(QStringLiteral("ok"), false);
        info.insert(QStringLiteral("path"), QString());
        info.insert(QStringLiteral("volumeCount"), 0);
        info.insert(QStringLiteral("fileCount"), static_cast<qint64>(0));
        info.insert(QStringLiteral("totalBytes"), static_cast<qint64>(0));
        return info;
    }

    info.insert(QStringLiteral("ok"), true);
    info.insert(QStringLiteral("path"), m_projectPath);

    const auto stats = m_db.projectStats();
    if (stats.has_value()) {
        info.insert(QStringLiteral("volumeCount"), stats->volumeCount);
        info.insert(QStringLiteral("fileCount"), static_cast<qint64>(stats->fileCount));
        info.insert(QStringLiteral("totalBytes"), static_cast<qint64>(stats->totalBytes));
    } else {
        info.insert(QStringLiteral("volumeCount"), 0);
        info.insert(QStringLiteral("fileCount"), static_cast<qint64>(0));
        info.insert(QStringLiteral("totalBytes"), static_cast<qint64>(0));
    }

    return info;
}

uint KatalogueDaemon::StartScan(const QString &rootPath) {
    if (!m_db.isOpen()) {
        if (!m_db.openProject(m_projectPath.isEmpty() ? defaultProjectPath()
                                                      : m_projectPath)) {
            if (calledFromDBus()) {
                sendErrorReply(QDBusError::Failed,
                               QStringLiteral("Failed to open catalog: %1").arg(m_db.lastErrorString()));
            }
            return 0;
        }
    }

    const QFileInfo rootInfo(rootPath);
    QStorageInfo storage(rootPath);

    VolumeInfo info;
    QString label;
    if (storage.isValid()) {
        label = storage.displayName();
    }
    if (label.trimmed().isEmpty()) {
        label = rootInfo.fileName();
    }
    if (label.trimmed().isEmpty()) {
        label = rootPath;
    }
    info.label = label;
    info.physicalHint = rootPath;
    if (storage.isValid()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        info.fsUuid = storage.deviceUuid();
#else
        info.fsUuid = QString::fromUtf8(storage.device());
#endif
        info.fsType = storage.fileSystemType();
        info.totalSize = static_cast<qint64>(storage.bytesTotal());
    }

    bool existingVolume = false;
    if (!info.fsUuid.trimmed().isEmpty()) {
        const auto existing = m_db.findVolumeByFsUuid(info.fsUuid);
        if (existing.has_value()) {
            info.id = existing->id;
            info.createdAt = existing->createdAt;
            existingVolume = true;
        }
    }
    if (!info.createdAt.isValid()) {
        info.createdAt = QDateTime::currentDateTimeUtc();
    }
    info.updatedAt = QDateTime::currentDateTimeUtc();

    ScanJob job;
    job.id = m_nextScanId++;
    job.rootPath = rootPath;
    job.status = ScanJob::Status::Pending;
    job.volumeInfo = info;
    job.options.includeHidden = false;
    job.options.followSymlinks = false;
    job.options.maxDepth = -1;
    job.existingVolume = existingVolume;
    m_jobs.insert(job.id, job);

    auto *worker = new QObject();
    worker->moveToThread(&m_scanThread);

    if (!m_scanThread.isRunning()) {
        m_scanThread.start();
    }

    QMetaObject::invokeMethod(worker, [this, jobId = job.id]() {
        runScan(jobId);
    }, Qt::QueuedConnection);

    connect(&m_scanThread, &QThread::finished, worker, &QObject::deleteLater);
    return job.id;
}

bool KatalogueDaemon::CancelScan(uint scanId) {
    if (!m_jobs.contains(scanId)) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Scan ID not found"));
        }
        return false;
    }
    m_scanner.requestCancel();
    m_jobs[scanId].status = ScanJob::Status::Cancelled;
    return true;
}

QVariantMap KatalogueDaemon::GetScanStatus(uint scanId) const {
    QVariantMap result;
    const auto it = m_jobs.constFind(scanId);
    if (it == m_jobs.constEnd()) {
        result.insert(QStringLiteral("status"), QStringLiteral("unknown"));
        return result;
    }

    const auto &job = it.value();
    result.insert(QStringLiteral("status"), statusToString(job.status));
    result.insert(QStringLiteral("state"), statusToString(job.status));
    if (!job.errorString.isEmpty()) {
        result.insert(QStringLiteral("error"), job.errorString);
    }
    result.insert(QStringLiteral("directories"), job.stats.directories);
    result.insert(QStringLiteral("files"), job.stats.files);
    result.insert(QStringLiteral("bytes"), static_cast<qint64>(job.stats.totalBytes));
    return result;
}

QVariantMap KatalogueDaemon::ListVolumes() const {
    QVariantMap payload;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        payload.insert(QStringLiteral("items"), QVariantList());
        return payload;
    }
    const auto volumes = m_db.listVolumes();
    QVariantList list;
    list.reserve(volumes.size());
    for (const auto &volume : volumes) {
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), volume.id);
        entry.insert(QStringLiteral("label"), volume.label);
        entry.insert(QStringLiteral("description"), volume.description);
        entry.insert(QStringLiteral("fs_uuid"), volume.fsUuid);
        entry.insert(QStringLiteral("fs_type"), volume.fsType);
        entry.insert(QStringLiteral("physical_hint"), volume.physicalHint);
        entry.insert(QStringLiteral("total_size"), static_cast<qint64>(volume.totalSize));
        entry.insert(QStringLiteral("created_at"), volume.createdAt.toSecsSinceEpoch());
        entry.insert(QStringLiteral("updated_at"), volume.updatedAt.toSecsSinceEpoch());
        list.append(entry);
    }
    payload.insert(QStringLiteral("items"), list);
    return payload;
}

QList<QVariantMap> KatalogueDaemon::ListDirectories(int volumeId, int parentId) const {
    QList<QVariantMap> entries;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return entries;
    }
    const auto directories = m_db.listDirectories(volumeId, parentId);
    entries.reserve(directories.size());
    for (const auto &dir : directories) {
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), dir.id);
        entry.insert(QStringLiteral("volume_id"), dir.volumeId);
        entry.insert(QStringLiteral("parent_id"), dir.parentId);
        entry.insert(QStringLiteral("name"), dir.name);
        entry.insert(QStringLiteral("full_path"), dir.fullPath);
        entries.append(entry);
    }
    return entries;
}

QList<QVariantMap> KatalogueDaemon::ListFiles(int directoryId) const {
    QList<QVariantMap> entries;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return entries;
    }
    const auto files = m_db.listFilesInDirectory(directoryId);
    QString volumeLabel;
    const auto directory = m_db.getDirectory(directoryId);
    if (directory.has_value()) {
        const auto label = m_db.getVolumeLabel(directory->volumeId);
        if (label.has_value()) {
            volumeLabel = label.value();
        }
    }
    entries.reserve(files.size());
    for (const auto &file : files) {
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), file.id);
        entry.insert(QStringLiteral("directory_id"), file.directoryId);
        entry.insert(QStringLiteral("name"), file.name);
        entry.insert(QStringLiteral("full_path"), file.fullPath);
        entry.insert(QStringLiteral("size"), static_cast<qint64>(file.size));
        entry.insert(QStringLiteral("mtime"), file.mtime.isValid()
                                              ? file.mtime.toString(Qt::ISODate)
                                              : QString());
        entry.insert(QStringLiteral("file_type"), file.fileType);
        entry.insert(QStringLiteral("volume_label"), volumeLabel);
        entries.append(entry);
    }
    return entries;
}

QVariantMap KatalogueDaemon::SearchByName(const QString &query, int limit, int offset) const {
    QVariantMap payload;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        payload.insert(QStringLiteral("items"), QVariantList());
        return payload;
    }
    const auto results = m_db.searchByName(query, limit, offset);
    QVariantList list;
    list.reserve(results.size());
    for (const auto &result : results) {
        QVariantMap entry;
        entry.insert(QStringLiteral("file_id"), result.fileId);
        entry.insert(QStringLiteral("file_name"), result.fileName);
        entry.insert(QStringLiteral("full_path"), result.fullPath);
        entry.insert(QStringLiteral("volume_label"), result.volumeLabel);
        entry.insert(QStringLiteral("size"), static_cast<qint64>(result.size));
        entry.insert(QStringLiteral("mtime"), result.mtime.toSecsSinceEpoch());
        list.append(entry);
    }
    payload.insert(QStringLiteral("items"), list);
    return payload;
}

QList<QVariantMap> KatalogueDaemon::Search(const QString &query,
                                           int volumeId,
                                           const QString &fileType,
                                           int limit,
                                           int offset) const {
    QList<QVariantMap> entries;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return entries;
    }
    KatalogueDatabase::SearchFilters filters;
    if (volumeId >= 0) {
        filters.volumeId = volumeId;
    }
    if (!fileType.trimmed().isEmpty()) {
        filters.fileType = fileType.toLower();
    }

    const auto results = m_db.search(query, filters, limit, offset);
    entries.reserve(results.size());
    for (const auto &result : results) {
        QVariantMap entry;
        entry.insert(QStringLiteral("fileId"), result.fileId);
        entry.insert(QStringLiteral("directoryId"), result.directoryId);
        entry.insert(QStringLiteral("volumeId"), result.volumeId);
        entry.insert(QStringLiteral("fileName"), result.fileName);
        entry.insert(QStringLiteral("fullPath"), result.fullPath);
        entry.insert(QStringLiteral("volumeLabel"), result.volumeLabel);
        entry.insert(QStringLiteral("fileType"), result.fileType);
        entry.insert(QStringLiteral("size"), static_cast<qint64>(result.size));
        entry.insert(QStringLiteral("mtime"), result.mtime.isValid()
                                             ? result.mtime.toString(Qt::ISODate)
                                             : QString());
        entries.append(entry);
    }
    return entries;
}

QString KatalogueDaemon::GetFileNote(int fileId) const {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return {};
    }
    const auto note = m_db.getNoteForFile(fileId);
    return note.value_or(QString());
}

void KatalogueDaemon::SetFileNote(int fileId, const QString &content) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.setNoteForFile(fileId, content);
}

QList<QVariantMap> KatalogueDaemon::GetFileTags(int fileId) const {
    QList<QVariantMap> entries;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return entries;
    }
    const auto tags = m_db.tagsForFile(fileId);
    entries.reserve(tags.size());
    for (const auto &tag : tags) {
        QVariantMap entry;
        entry.insert(QStringLiteral("key"), tag.first);
        entry.insert(QStringLiteral("value"), tag.second);
        entries.append(entry);
    }
    return entries;
}

void KatalogueDaemon::AddFileTag(int fileId, const QString &key, const QString &value) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.addTagToFile(fileId, key, value);
}

void KatalogueDaemon::RemoveFileTag(int fileId, const QString &key, const QString &value) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.removeTagFromFile(fileId, key, value);
}

QList<QVariantMap> KatalogueDaemon::ListVirtualFolders(int parentId) const {
    QList<QVariantMap> entries;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return entries;
    }
    const auto folders = m_db.listVirtualFolders(parentId);
    entries.reserve(folders.size());
    for (const auto &folder : folders) {
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), folder.id);
        entry.insert(QStringLiteral("parentId"), folder.parentId);
        entry.insert(QStringLiteral("name"), folder.name);
        entries.append(entry);
    }
    return entries;
}

int KatalogueDaemon::CreateVirtualFolder(const QString &name, int parentId) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return -1;
    }
    return m_db.createVirtualFolder(name, parentId);
}

void KatalogueDaemon::RenameVirtualFolder(int folderId, const QString &newName) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.renameVirtualFolder(folderId, newName);
}

void KatalogueDaemon::DeleteVirtualFolder(int folderId) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.deleteVirtualFolder(folderId);
}

QList<QVariantMap> KatalogueDaemon::ListVirtualFolderItems(int folderId) const {
    QList<QVariantMap> entries;
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return entries;
    }
    const auto items = m_db.listVirtualFolderItems(folderId);
    entries.reserve(items.size());
    for (const auto &item : items) {
        QVariantMap entry;
        entry.insert(QStringLiteral("fileId"), item.fileId);
        entry.insert(QStringLiteral("directoryId"), item.directoryId);
        entry.insert(QStringLiteral("volumeId"), item.volumeId);
        entry.insert(QStringLiteral("fileName"), item.fileName);
        entry.insert(QStringLiteral("fullPath"), item.fullPath);
        entry.insert(QStringLiteral("volumeLabel"), item.volumeLabel);
        entry.insert(QStringLiteral("fileType"), item.fileType);
        entry.insert(QStringLiteral("size"), static_cast<qint64>(item.size));
        entry.insert(QStringLiteral("mtime"), item.mtime.isValid()
                                             ? item.mtime.toString(Qt::ISODate)
                                             : QString());
        entries.append(entry);
    }
    return entries;
}

void KatalogueDaemon::AddFileToVirtualFolder(int folderId, int fileId) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.addFileToVirtualFolder(folderId, fileId);
}

void KatalogueDaemon::RemoveFileFromVirtualFolder(int folderId, int fileId) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.removeFileFromVirtualFolder(folderId, fileId);
}

void KatalogueDaemon::RenameVolume(int volumeId, const QString &newLabel) {
    if (!m_db.isOpen()) {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::Failed, QStringLiteral("Database is not open"));
        }
        return;
    }
    m_db.renameVolume(volumeId, newLabel);
}

void KatalogueDaemon::runScan(uint scanId) {
    auto it = m_jobs.find(scanId);
    if (it == m_jobs.end()) {
        return;
    }

    it->status = ScanJob::Status::Running;
    it->errorString.clear();
    emit ScanProgress(scanId, it->rootPath, it->stats.directories, it->stats.files, it->stats.totalBytes);

    if (it->existingVolume) {
        if (!m_db.clearVolumeContents(it->volumeInfo.id)) {
            it->status = ScanJob::Status::Failed;
            emit ScanFinished(scanId, statusToString(it->status));
            return;
        }
    }

    const bool ok = m_scanner.scan(it->rootPath,
                                   m_db,
                                   it->volumeInfo,
                                   it->options,
                                   [this, scanId](const QString &path, const ScanStats &stats) {
        auto jobIt = m_jobs.find(scanId);
        if (jobIt == m_jobs.end()) {
            return false;
        }
        jobIt->stats = stats;
        emit ScanProgress(scanId, path, stats.directories, stats.files, stats.totalBytes);
        return true;
    });

    if (!ok) {
        if (m_scanner.isCancelRequested() || it->status == ScanJob::Status::Cancelled) {
            it->status = ScanJob::Status::Cancelled;
            it->errorString = QStringLiteral("Scan cancelled");
        } else {
            it->status = ScanJob::Status::Failed;
            it->errorString = QStringLiteral("Scan failed");
        }
    } else if (it->status != ScanJob::Status::Cancelled) {
        it->status = ScanJob::Status::Finished;
    }

    emit ScanFinished(scanId, statusToString(it->status));
}

QString KatalogueDaemon::statusToString(ScanJob::Status status) const {
    switch (status) {
    case ScanJob::Status::Pending:
        return QStringLiteral("pending");
    case ScanJob::Status::Running:
        return QStringLiteral("running");
    case ScanJob::Status::Finished:
        return QStringLiteral("finished");
    case ScanJob::Status::Cancelled:
        return QStringLiteral("cancelled");
    case ScanJob::Status::Failed:
        return QStringLiteral("failed");
    }
    return QStringLiteral("unknown");
}
