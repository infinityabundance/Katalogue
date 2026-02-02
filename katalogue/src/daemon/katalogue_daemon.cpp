#include "katalogue_daemon.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

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

bool KatalogueDaemon::OpenProject(const QString &path) {
    if (path.trimmed().isEmpty()) {
        return false;
    }
    m_projectPath = path;
    return m_db.openProject(m_projectPath);
}

uint KatalogueDaemon::StartScan(const QString &rootPath) {
    if (!m_db.isOpen()) {
        if (!m_db.openProject(m_projectPath.isEmpty() ? defaultProjectPath()
                                                      : m_projectPath)) {
            return 0;
        }
    }

    ScanJob job;
    job.id = m_nextScanId++;
    job.rootPath = rootPath;
    job.status = ScanJob::Status::Pending;
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

    VolumeInfo info;
    info.label = QFileInfo(it->rootPath).fileName();
    info.createdAt = QDateTime::currentDateTimeUtc();

    const bool ok = m_scanner.scan(it->rootPath, m_db, info, {}, [this, scanId](const QString &path,
                                                                              const ScanStats &stats) {
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
