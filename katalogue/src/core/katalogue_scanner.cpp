#include "katalogue_scanner.h"

#include "katalogue_database.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

KatalogueScanner::KatalogueScanner(KatalogueDatabase *db, QObject *parent)
    : QObject(parent)
    , m_db(db) {}

void KatalogueScanner::setOptions(const Options &options) {
    m_options = options;
}

void KatalogueScanner::scanVolume(const QString &mountPoint, VolumeInfo volumeInfo) {
    QFileInfo rootInfo(mountPoint);
    if (!rootInfo.exists() || !rootInfo.isDir()) {
        emit finished(false, QStringLiteral("Mount point is not a directory: %1").arg(mountPoint));
        return;
    }

    if (!m_db || !m_db->isOpen()) {
        emit finished(false, QStringLiteral("Database is not open"));
        return;
    }

    const auto now = QDateTime::currentDateTimeUtc();
    if (!volumeInfo.createdAt.isValid()) {
        volumeInfo.createdAt = now;
    }
    volumeInfo.updatedAt = now;

    const int volumeId = m_db->upsertVolume(volumeInfo);
    if (volumeId <= 0) {
        emit finished(false, QStringLiteral("Failed to insert volume"));
        return;
    }

    m_directoryCache.clear();

    DirectoryInfo rootDir;
    rootDir.volumeId = volumeId;
    rootDir.parentId = -1;
    rootDir.name = QStringLiteral("/");
    rootDir.fullPath = QStringLiteral("/");
    const int rootId = m_db->upsertDirectory(rootDir);
    m_directoryCache.insert(QStringLiteral("/"), rootId);

    quint64 filesProcessed = 0;
    const QDir baseDir(mountPoint);

    QDirIterator it(mountPoint,
                    QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        const QFileInfo entryInfo = it.fileInfo();
        const QString absPath = entryInfo.absoluteFilePath();
        const QString relPath = baseDir.relativeFilePath(absPath);
        if (relPath == QStringLiteral(".")) {
            continue;
        }
        const QString fullPath = QStringLiteral("/") + relPath;

        if (entryInfo.isDir()) {
            const QString parentRelPath = baseDir.relativeFilePath(entryInfo.absolutePath());
            const QString parentFullPath = parentRelPath == QStringLiteral(".")
                                               ? QStringLiteral("/")
                                               : QStringLiteral("/") + parentRelPath;
            const int parentId = m_directoryCache.value(parentFullPath, rootId);

            DirectoryInfo dirInfo;
            dirInfo.volumeId = volumeId;
            dirInfo.parentId = parentId;
            dirInfo.name = entryInfo.fileName();
            dirInfo.fullPath = fullPath;

            const int dirId = m_db->upsertDirectory(dirInfo);
            if (dirId > 0) {
                m_directoryCache.insert(fullPath, dirId);
            }
            continue;
        }

        if (entryInfo.isFile()) {
            if (shouldSkip(relPath)) {
                continue;
            }

            const QString parentRelPath = baseDir.relativeFilePath(entryInfo.absolutePath());
            const QString parentFullPath = parentRelPath == QStringLiteral(".")
                                               ? QStringLiteral("/")
                                               : QStringLiteral("/") + parentRelPath;
            const int dirId = m_directoryCache.value(parentFullPath, rootId);

            FileInfo fileInfo;
            fileInfo.directoryId = dirId;
            fileInfo.name = entryInfo.fileName();
            fileInfo.size = entryInfo.size();
            fileInfo.mtime = entryInfo.lastModified().toUTC();
            fileInfo.ctime = entryInfo.birthTime().isValid()
                                 ? entryInfo.birthTime().toUTC()
                                 : entryInfo.created().toUTC();
            fileInfo.fileType = detectFileType(entryInfo.fileName());
            if (m_options.computeHashes) {
                fileInfo.hash = computeHash(absPath);
            }

            m_db->insertFile(fileInfo);
            ++filesProcessed;

            if (filesProcessed % 100 == 0) {
                emit progress(-1, filesProcessed);
            }
        }
    }

    emit progress(100, filesProcessed);
    emit finished(true, QString());
}

bool KatalogueScanner::shouldSkip(const QString &relativePath) const {
    if (m_options.excludePatterns.isEmpty()) {
        return false;
    }

    const QString fileName = QFileInfo(relativePath).fileName();
    for (const auto &pattern : m_options.excludePatterns) {
        if (QDir::match(pattern, relativePath) || QDir::match(pattern, fileName)) {
            return true;
        }
    }
    return false;
}

QString KatalogueScanner::detectFileType(const QString &fileName) const {
    const int dotIndex = fileName.lastIndexOf('.');
    if (dotIndex <= 0 || dotIndex >= fileName.size() - 1) {
        return {};
    }
    return fileName.mid(dotIndex + 1).toLower();
}

QString KatalogueScanner::computeHash(const QString &filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for hashing" << filePath;
        return {};
    }

    QCryptographicHash hasher(QCryptographicHash::Sha256);
    if (!hasher.addData(&file)) {
        qWarning() << "Failed to hash file" << filePath;
        return {};
    }

    return QString::fromLatin1(hasher.result().toHex());
}
