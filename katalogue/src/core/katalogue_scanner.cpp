#include "katalogue_scanner.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QStorageInfo>

KatalogueScanner::KatalogueScanner() = default;

bool KatalogueScanner::scan(const QString &rootPath,
                            KatalogueDatabase &db,
                            VolumeInfo volumeInfo,
                            const ScanOptions &options,
                            ProgressCallback progress) {
    m_cancelled.store(false);
    m_cancelRequested.store(false);

    if (!db.isOpen()) {
        return false;
    }

    const QFileInfo rootInfo(rootPath);
    if (!rootInfo.exists() || !rootInfo.isDir()) {
        return false;
    }

    if (volumeInfo.label.trimmed().isEmpty()) {
        volumeInfo.label = rootInfo.fileName().isEmpty()
                               ? QStringLiteral("Root")
                               : rootInfo.fileName();
    }

    if (!volumeInfo.createdAt.isValid()) {
        volumeInfo.createdAt = QDateTime::currentDateTimeUtc();
    }
    volumeInfo.updatedAt = QDateTime::currentDateTimeUtc();

    QStorageInfo storage(rootPath);
    if (storage.isValid()) {
        if (volumeInfo.fsUuid.isEmpty()) {
            volumeInfo.fsUuid = storage.device();
        }
        if (volumeInfo.fsType.isEmpty()) {
            volumeInfo.fsType = storage.fileSystemType();
        }
        if (volumeInfo.totalSize <= 0) {
            volumeInfo.totalSize = static_cast<qint64>(storage.bytesTotal());
        }
    }

    const int volumeId = db.upsertVolume(volumeInfo);
    if (volumeId < 0) {
        return false;
    }

    QHash<QString, int> directoryIds;
    DirectoryInfo rootDir;
    rootDir.volumeId = volumeId;
    rootDir.name = QStringLiteral("/");
    rootDir.fullPath = QStringLiteral("/");
    const int rootId = db.upsertDirectory(rootDir);
    if (rootId < 0) {
        return false;
    }
    directoryIds.insert(QStringLiteral("/"), rootId);

    QDir rootDirHandle(rootPath);
    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot;
    if (options.includeHidden) {
        filters |= QDir::Hidden;
    }
    if (!options.followSymlinks) {
        filters |= QDir::NoSymLinks;
    }

    QDirIterator::IteratorFlags iteratorFlags = QDirIterator::Subdirectories;
    if (options.followSymlinks) {
        iteratorFlags |= QDirIterator::FollowSymlinks;
    }

    QDirIterator it(rootPath, filters, iteratorFlags);
    QMimeDatabase mimeDb;
    ScanStats stats;

    while (it.hasNext()) {
        if (m_cancelled.load() || m_cancelRequested.load(std::memory_order_relaxed)) {
            return false;
        }

        const QString path = it.next();
        const QFileInfo info = it.fileInfo();
        const QString relativePath = rootDirHandle.relativeFilePath(path);

        if (relativePath.isEmpty() || relativePath == QStringLiteral(".")) {
            continue;
        }

        if (!withinDepth(relativePath, options.maxDepth)) {
            continue;
        }

        if (isExcluded(relativePath, info.fileName(), options)) {
            continue;
        }

        if (info.isSymLink()) {
            continue;
        }

        if (info.isDir()) {
            DirectoryInfo dirInfo;
            dirInfo.volumeId = volumeId;
            dirInfo.name = info.fileName();
            dirInfo.fullPath = QDir::cleanPath(QStringLiteral("/") + relativePath);

            const QString parentRelative = QDir::cleanPath(QStringLiteral("/") +
                                                           QFileInfo(relativePath).path());
            dirInfo.parentId = directoryIds.value(parentRelative, rootId);

            const int dirId = db.upsertDirectory(dirInfo);
            if (dirId < 0) {
                return false;
            }
            directoryIds.insert(dirInfo.fullPath, dirId);
            stats.directories += 1;
        } else if (info.isFile()) {
            const QString parentRelative = QDir::cleanPath(QStringLiteral("/") +
                                                           QFileInfo(relativePath).path());
            const int parentId = directoryIds.value(parentRelative, rootId);

            FileInfo fileInfo;
            fileInfo.directoryId = parentId;
            fileInfo.name = info.fileName();
            fileInfo.size = info.size();
            fileInfo.mtime = info.lastModified().toUTC();
            fileInfo.ctime = info.birthTime().isValid() ? info.birthTime().toUTC()
                                                        : info.metadataChangeTime().toUTC();
            fileInfo.fileType = mimeDb.mimeTypeForFile(info).name();

            if (db.upsertFile(fileInfo) < 0) {
                return false;
            }

            stats.files += 1;
            stats.totalBytes += fileInfo.size;
        } else {
            continue;
        }

        if (progress) {
            if (!progress(path, stats)) {
                return false;
            }
        }
    }

    return true;
}

void KatalogueScanner::cancel() {
    m_cancelled.store(true);
    requestCancel();
}

void KatalogueScanner::requestCancel() {
    m_cancelRequested.store(true, std::memory_order_relaxed);
}

bool KatalogueScanner::isCancelRequested() const {
    return m_cancelRequested.load(std::memory_order_relaxed);
}

bool KatalogueScanner::isExcluded(const QString &relativePath,
                                  const QString &name,
                                  const ScanOptions &options) const {
    if (options.excludePatterns.isEmpty()) {
        return false;
    }

    if (QDir::match(options.excludePatterns, relativePath)) {
        return true;
    }

    return QDir::match(options.excludePatterns, name);
}

bool KatalogueScanner::withinDepth(const QString &relativePath, int maxDepth) const {
    if (maxDepth < 0) {
        return true;
    }
    const QString cleaned = QDir::cleanPath(relativePath);
    if (cleaned.isEmpty() || cleaned == QStringLiteral(".")) {
        return true;
    }
    const QStringList parts = cleaned.split('/', Qt::SkipEmptyParts);
    return parts.size() <= maxDepth;
}
