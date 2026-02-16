#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QTemporaryDir>

#include "katalogue_database.h"

static void benchInsert(KatalogueDatabase &db, int fileCount) {
    QElapsedTimer timer;
    timer.start();

    VolumeInfo volume;
    volume.label = QStringLiteral("Bench Volume");
    const int volumeId = db.upsertVolume(volume);
    if (volumeId < 0) {
        qCritical() << "Failed to create volume";
        return;
    }

    DirectoryInfo root;
    root.volumeId = volumeId;
    root.parentId = -1;
    root.name = QStringLiteral("/");
    root.fullPath = QStringLiteral("/");
    const int rootId = db.upsertDirectory(root);
    if (rootId < 0) {
        qCritical() << "Failed to create root directory";
        return;
    }

    // Create subdirectories (1 per 100 files)
    const int dirCount = fileCount / 100;
    QList<int> dirIds;
    dirIds.reserve(dirCount + 1);
    dirIds.append(rootId);

    db.beginBatch();
    for (int d = 0; d < dirCount; ++d) {
        DirectoryInfo dir;
        dir.volumeId = volumeId;
        dir.parentId = rootId;
        dir.name = QStringLiteral("dir_%1").arg(d);
        dir.fullPath = QStringLiteral("/dir_%1").arg(d);
        const int dirId = db.upsertDirectory(dir);
        if (dirId >= 0) {
            dirIds.append(dirId);
        }
    }
    db.endBatch();

    const qint64 dirMs = timer.elapsed();
    qInfo() << "Created" << dirIds.size() << "directories in" << dirMs << "ms";

    timer.restart();

    db.beginBatch();
    for (int i = 0; i < fileCount; ++i) {
        FileInfo file;
        file.directoryId = dirIds.at(i % dirIds.size());
        file.name = QStringLiteral("file_%1.dat").arg(i);
        file.size = (i % 1000) * 1024;
        file.mtime = QDateTime::currentDateTimeUtc();
        file.fileType = QStringLiteral("application/octet-stream");

        if (db.upsertFile(file) < 0) {
            qWarning() << "Insert failed at file" << i;
            break;
        }

        if (i > 0 && i % 500 == 0) {
            db.endBatch();
            db.beginBatch();
        }
    }
    db.endBatch();

    const qint64 insertMs = timer.elapsed();
    qInfo() << "Inserted" << fileCount << "files in" << insertMs << "ms"
            << "(" << (fileCount * 1000 / qMax(insertMs, qint64(1))) << "files/sec)";
}

static void benchSearch(KatalogueDatabase &db) {
    QElapsedTimer timer;
    timer.start();

    KatalogueDatabase::SearchFilters filters;
    const auto results = db.search(QStringLiteral("file_42"), filters, 100, 0);

    const qint64 searchMs = timer.elapsed();
    qInfo() << "Search returned" << results.size() << "results in" << searchMs << "ms";
}

static void benchListAllFiles(KatalogueDatabase &db) {
    QElapsedTimer timer;
    timer.start();

    const auto all = db.listAllFiles();

    const qint64 listMs = timer.elapsed();
    qInfo() << "listAllFiles returned" << all.size() << "results in" << listMs << "ms";
}

static void benchProjectStats(KatalogueDatabase &db) {
    QElapsedTimer timer;
    timer.start();

    const auto stats = db.projectStats();

    const qint64 statsMs = timer.elapsed();
    if (stats.has_value()) {
        qInfo() << "projectStats:" << stats->volumeCount << "volumes,"
                << stats->fileCount << "files,"
                << stats->totalBytes << "bytes in" << statsMs << "ms";
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    const int fileCount = 100000;
    qInfo() << "=== Katalogue Performance Benchmark ===";
    qInfo() << "File count:" << fileCount;

    QTemporaryDir tmp;
    if (!tmp.isValid()) {
        qCritical() << "Failed to create temp dir";
        return 1;
    }

    const QString dbPath = tmp.filePath("bench.kdcatalog");
    KatalogueDatabase db;
    if (!db.openProject(dbPath)) {
        qCritical() << "Failed to open project:" << db.lastErrorString();
        return 1;
    }

    benchInsert(db, fileCount);
    benchSearch(db);
    benchListAllFiles(db);
    benchProjectStats(db);

    qInfo() << "=== Benchmark complete ===";
    return 0;
}
