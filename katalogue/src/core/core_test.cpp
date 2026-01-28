#include "katalogue_database.h"
#include "katalogue_scanner.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    KatalogueDatabase db;
    if (!db.openProject(QStringLiteral("./test.kdcatalog"))) {
        qWarning() << "Failed to open test database";
        return 1;
    }

    if (argc > 1) {
        const QString mountPoint = QString::fromLocal8Bit(argv[1]);
        QFileInfo info(mountPoint);

        VolumeInfo volume;
        volume.label = info.exists() ? info.fileName() : QStringLiteral("Scanned Volume");
        if (volume.label.isEmpty()) {
            volume.label = QStringLiteral("Scanned Volume");
        }

        KatalogueScanner scanner(&db);
        QObject::connect(&scanner, &KatalogueScanner::progress, [](int percent, quint64 files) {
            qDebug() << "Progress:" << percent << "files:" << files;
        });
        bool success = false;
        QString errorString;
        QObject::connect(&scanner, &KatalogueScanner::finished,
                         [&](bool ok, const QString &error) {
                             success = ok;
                             errorString = error;
                         });

        scanner.scanVolume(mountPoint, volume);

        if (!success) {
            qWarning() << "Scan failed:" << errorString;
            return 1;
        }

        qInfo() << "Scan complete.";
        return 0;
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
    db.insertFile(file);

    const auto results = db.searchByName(QStringLiteral("test"));
    for (const auto &result : results) {
        qDebug() << result.fileId << result.fileName << result.fullPath << result.volumeLabel;
    }

    return 0;
}
