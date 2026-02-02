#include <QtTest>

#include "katalogue_database.h"

class KatalogueDatabaseTest : public QObject {
    Q_OBJECT
private slots:
    void testOpenProject();
    void testInsertVolumeAndPaths();
    void testSearch();
};

void KatalogueDatabaseTest::testOpenProject() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());
    const QString dbPath = tmp.filePath("test.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));
    QVERIFY(db.isOpen());
    QCOMPARE(db.checkSchema(), KatalogueDatabase::SchemaStatus::Ok);
}

void KatalogueDatabaseTest::testInsertVolumeAndPaths() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());
    const QString dbPath = tmp.filePath("paths.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    VolumeInfo volume;
    volume.label = QStringLiteral("Test Volume");
    const int volumeId = db.upsertVolume(volume);
    QVERIFY(volumeId >= 0);

    DirectoryInfo root;
    root.volumeId = volumeId;
    root.parentId = -1;
    root.name = QStringLiteral("/");
    root.fullPath = QStringLiteral("/");
    const int rootId = db.upsertDirectory(root);
    QVERIFY(rootId >= 0);

    DirectoryInfo docs;
    docs.volumeId = volumeId;
    docs.parentId = rootId;
    docs.name = QStringLiteral("docs");
    docs.fullPath = QStringLiteral("/docs");
    const int docsId = db.upsertDirectory(docs);
    QVERIFY(docsId >= 0);

    FileInfo file;
    file.directoryId = docsId;
    file.name = QStringLiteral("notes.txt");
    file.size = 123;
    file.mtime = QDateTime::currentDateTimeUtc();
    file.fileType = QStringLiteral("text/plain");
    const int fileId = db.upsertFile(file);
    QVERIFY(fileId >= 0);

    const auto volumes = db.listVolumes();
    QCOMPARE(volumes.size(), 1);
    QCOMPARE(volumes.first().label, QStringLiteral("Test Volume"));

    const auto roots = db.listDirectories(volumeId, -1);
    QVERIFY(!roots.isEmpty());

    const auto files = db.listFilesInDirectory(docsId);
    QCOMPARE(files.size(), 1);
    QCOMPARE(files.first().name, QStringLiteral("notes.txt"));
}

void KatalogueDatabaseTest::testSearch() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());
    const QString dbPath = tmp.filePath("search.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    VolumeInfo volume;
    volume.label = QStringLiteral("Search Volume");
    const int volumeId = db.upsertVolume(volume);
    QVERIFY(volumeId >= 0);

    DirectoryInfo root;
    root.volumeId = volumeId;
    root.parentId = -1;
    root.name = QStringLiteral("/");
    root.fullPath = QStringLiteral("/");
    const int rootId = db.upsertDirectory(root);
    QVERIFY(rootId >= 0);

    FileInfo fileA;
    fileA.directoryId = rootId;
    fileA.name = QStringLiteral("report_alpha.txt");
    fileA.size = 10;
    fileA.mtime = QDateTime::currentDateTimeUtc();
    fileA.fileType = QStringLiteral("text/plain");
    QVERIFY(db.upsertFile(fileA) >= 0);

    FileInfo fileB = fileA;
    fileB.name = QStringLiteral("image.png");
    fileB.fileType = QStringLiteral("image/png");
    QVERIFY(db.upsertFile(fileB) >= 0);

    KatalogueDatabase::SearchFilters filters;
    filters.volumeId = volumeId;
    const auto results = db.search(QStringLiteral("report"), filters, 50, 0);
    QVERIFY(!results.isEmpty());
    QCOMPARE(results.first().fileName, QStringLiteral("report_alpha.txt"));
}

QTEST_MAIN(KatalogueDatabaseTest)
#include "tst_katalogue_database.moc"
