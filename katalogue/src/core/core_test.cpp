#include "katalogue_database.h"
#include "katalogue_scanner.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    KatalogueDatabase db;
    if (!db.openProject(QStringLiteral("./test.kdcatalog"))) {
        qWarning() << "Failed to open test database";
        return 1;
    }

    VolumeInfo volume;
    volume.label = QStringLiteral("Test Volume");
    volume.description = QStringLiteral("Sample dataset");
    volume.fsUuid = QStringLiteral("uuid-1234");
    volume.fsType = QStringLiteral("ext4");
    volume.physicalHint = QStringLiteral("Shelf A");
    volume.totalSize = 1024 * 1024;
    const int volumeId = db.upsertVolume(volume);

    DirectoryInfo root;
    root.volumeId = volumeId;
    root.name = QStringLiteral("/");
    root.fullPath = QStringLiteral("/");
    const int rootId = db.upsertDirectory(root);

    DirectoryInfo docs;
    docs.volumeId = volumeId;
    docs.parentId = rootId;
    docs.name = QStringLiteral("docs");
    docs.fullPath = QStringLiteral("/docs");
    const int docsId = db.upsertDirectory(docs);

    FileInfo file;
    file.directoryId = docsId;
    file.name = QStringLiteral("test_notes.txt");
    file.size = 4096;
    file.fileType = QStringLiteral("text/plain");
    file.mtime = QDateTime::currentDateTimeUtc();
    file.ctime = file.mtime;
    const int fileId = db.insertFile(file);
    if (fileId < 0) {
        qWarning() << "Failed to insert file";
        return 1;
    }

    const auto results = db.searchByName(QStringLiteral("test"));
    for (const auto &result : results) {
        qDebug() << result.fileId << result.fileName << result.fullPath << result.volumeLabel;
    }

    if (results.isEmpty()) {
        qWarning() << "Search returned no results";
        return 1;
    }

    FileInfo updated = file;
    updated.id = fileId;
    updated.name = QStringLiteral("renamed_notes.txt");
    if (db.upsertFile(updated) < 0) {
        qWarning() << "Failed to update file";
        return 1;
    }

    if (!db.searchByName(QStringLiteral("renamed")).size()) {
        qWarning() << "Renamed file not found in search";
        return 1;
    }

    if (!db.deleteFile(fileId)) {
        qWarning() << "Failed to delete file";
        return 1;
    }

    if (!db.searchByName(QStringLiteral("renamed")).isEmpty()) {
        qWarning() << "Deleted file still appears in search";
        return 1;
    }

    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        qWarning() << "Failed to create temp directory";
        return 1;
    }

    const QString docsPath = tempDir.path() + QStringLiteral("/docs");
    const QString mediaPath = tempDir.path() + QStringLiteral("/media");
    if (!QDir().mkpath(docsPath) || !QDir().mkpath(mediaPath)) {
        qWarning() << "Failed to create test folders";
        return 1;
    }

    QFile readmeFile(docsPath + QStringLiteral("/readme.txt"));
    if (!readmeFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create test file";
        return 1;
    }
    readmeFile.write("hello");
    readmeFile.close();

    QFile imageFile(mediaPath + QStringLiteral("/photo.png"));
    if (!imageFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create test file";
        return 1;
    }
    imageFile.write("img");
    imageFile.close();

    KatalogueDatabase scanDb;
    const QString scanDbPath = tempDir.path() + QStringLiteral("/scan.kdcatalog");
    if (!scanDb.openProject(scanDbPath)) {
        qWarning() << "Failed to open scan database";
        return 1;
    }

    KatalogueScanner scanner;
    ScanOptions options;
    options.excludePatterns = {QStringLiteral("*.png")};
    if (!scanner.scan(tempDir.path(), scanDb, {}, options)) {
        qWarning() << "Scan failed";
        return 1;
    }

    if (scanDb.searchByName(QStringLiteral("readme")).isEmpty()) {
        qWarning() << "Scanner did not index readme";
        return 1;
    }

    if (!scanDb.searchByName(QStringLiteral("photo")).isEmpty()) {
        qWarning() << "Exclude pattern did not skip png";
        return 1;
    }

    return 0;
}
