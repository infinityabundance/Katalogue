#include "katalogue_daemon.h"

#include <QDateTime>
#include <QDir>
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
    if (!m_projectPath.isEmpty()) {
        QDir().mkpath(QFileInfo(m_projectPath).absolutePath());
        m_db.openProject(m_projectPath);
    }

    m_scanThread.setObjectName(QStringLiteral("katalogue-scan-thread"));
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
        return false;
    }
    m_scanner.cancel();
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
    result.insert(QStringLiteral("directories"), job.stats.directories);
    result.insert(QStringLiteral("files"), job.stats.files);
    result.insert(QStringLiteral("bytes"), static_cast<qint64>(job.stats.totalBytes));
    return result;
}

QVariantMap KatalogueDaemon::ListVolumes() const {
    QVariantMap payload;
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
    const auto files = m_db.listFilesInDirectory(directoryId);
    entries.reserve(files.size());
    for (const auto &file : files) {
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), file.id);
        entry.insert(QStringLiteral("directory_id"), file.directoryId);
        entry.insert(QStringLiteral("name"), file.name);
        entry.insert(QStringLiteral("size"), static_cast<qint64>(file.size));
        entry.insert(QStringLiteral("mtime"), file.mtime.isValid()
                                              ? file.mtime.toString(Qt::ISODate)
                                              : QString());
        entry.insert(QStringLiteral("file_type"), file.fileType);
        entries.append(entry);
    }
    return entries;
}

QVariantMap KatalogueDaemon::SearchByName(const QString &query, int limit, int offset) const {
    QVariantMap payload;
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

void KatalogueDaemon::runScan(uint scanId) {
    auto it = m_jobs.find(scanId);
    if (it == m_jobs.end()) {
        return;
    }

    it->status = ScanJob::Status::Running;
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
        if (it->status != ScanJob::Status::Cancelled) {
            it->status = ScanJob::Status::Failed;
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
