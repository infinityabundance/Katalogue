#include <QtTest>

#include "katalogue_database.h"

class KatalogueDatabaseTest : public QObject {
    Q_OBJECT
private slots:
    void testOpenProject();
    void testInsertVolumeAndPaths();
    void testSearch();
    void testNotesAndTags();
    void testVirtualFolders();
    void testProjectStatsAndListAllFiles();
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

void KatalogueDatabaseTest::testNotesAndTags() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());
    const QString dbPath = tmp.filePath("notes_tags.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    VolumeInfo volume;
    volume.label = QStringLiteral("NTVolume");
    const int volumeId = db.upsertVolume(volume);
    QVERIFY(volumeId >= 0);

    DirectoryInfo root;
    root.volumeId = volumeId;
    root.parentId = -1;
    root.name = QStringLiteral("/");
    root.fullPath = QStringLiteral("/");
    const int rootId = db.upsertDirectory(root);
    QVERIFY(rootId >= 0);

    FileInfo file;
    file.directoryId = rootId;
    file.name = QStringLiteral("readme.md");
    file.size = 256;
    file.mtime = QDateTime::currentDateTimeUtc();
    file.fileType = QStringLiteral("text/markdown");
    const int fileId = db.upsertFile(file);
    QVERIFY(fileId >= 0);

    // File notes: set, get, overwrite, clear
    QVERIFY(!db.getNoteForFile(fileId).has_value());
    QVERIFY(db.setNoteForFile(fileId, QStringLiteral("First note")));
    QCOMPARE(db.getNoteForFile(fileId).value(), QStringLiteral("First note"));
    QVERIFY(db.setNoteForFile(fileId, QStringLiteral("Updated note")));
    QCOMPARE(db.getNoteForFile(fileId).value(), QStringLiteral("Updated note"));
    QVERIFY(db.setNoteForFile(fileId, QString()));
    QVERIFY(!db.getNoteForFile(fileId).has_value());

    // Directory notes
    QVERIFY(!db.getNoteForDirectory(rootId).has_value());
    QVERIFY(db.setNoteForDirectory(rootId, QStringLiteral("Root note")));
    QCOMPARE(db.getNoteForDirectory(rootId).value(), QStringLiteral("Root note"));
    QVERIFY(db.setNoteForDirectory(rootId, QString()));
    QVERIFY(!db.getNoteForDirectory(rootId).has_value());

    // Tags: add, list, remove
    QVERIFY(db.tagsForFile(fileId).isEmpty());
    QVERIFY(db.addTagToFile(fileId, QStringLiteral("category"), QStringLiteral("docs")));
    QVERIFY(db.addTagToFile(fileId, QStringLiteral("priority"), QStringLiteral("high")));
    QVERIFY(db.addTagToFile(fileId, QStringLiteral("category"), QStringLiteral("archive")));

    auto tags = db.tagsForFile(fileId);
    QCOMPARE(tags.size(), 3);

    // Remove one tag and verify
    QVERIFY(db.removeTagFromFile(fileId, QStringLiteral("priority"), QStringLiteral("high")));
    tags = db.tagsForFile(fileId);
    QCOMPARE(tags.size(), 2);

    // Remove non-existent tag succeeds gracefully
    QVERIFY(db.removeTagFromFile(fileId, QStringLiteral("nonexistent"), QStringLiteral("value")));
    QCOMPARE(db.tagsForFile(fileId).size(), 2);

    // Adding duplicate tag is idempotent
    QVERIFY(db.addTagToFile(fileId, QStringLiteral("category"), QStringLiteral("docs")));
    QCOMPARE(db.tagsForFile(fileId).size(), 2);

    // Empty key is rejected
    QVERIFY(!db.addTagToFile(fileId, QString(), QStringLiteral("val")));
}

void KatalogueDatabaseTest::testVirtualFolders() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());
    const QString dbPath = tmp.filePath("vfolders.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    // Create root-level virtual folder
    const int folderId = db.createVirtualFolder(QStringLiteral("My Collection"), -1);
    QVERIFY(folderId >= 0);

    // List root virtual folders
    auto folders = db.listVirtualFolders(-1);
    QCOMPARE(folders.size(), 1);
    QCOMPARE(folders.first().name, QStringLiteral("My Collection"));

    // Rename
    QVERIFY(db.renameVirtualFolder(folderId, QStringLiteral("Renamed Collection")));
    folders = db.listVirtualFolders(-1);
    QCOMPARE(folders.first().name, QStringLiteral("Renamed Collection"));

    // Get folder
    auto folder = db.getVirtualFolder(folderId);
    QVERIFY(folder.has_value());
    QCOMPARE(folder->name, QStringLiteral("Renamed Collection"));

    // Nested folder
    const int childId = db.createVirtualFolder(QStringLiteral("Sub Folder"), folderId);
    QVERIFY(childId >= 0);
    auto children = db.listVirtualFolders(folderId);
    QCOMPARE(children.size(), 1);
    QCOMPARE(children.first().name, QStringLiteral("Sub Folder"));

    // Root still has 1 folder
    QCOMPARE(db.listVirtualFolders(-1).size(), 1);

    // Add files to virtual folder
    VolumeInfo volume;
    volume.label = QStringLiteral("VFVolume");
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
    fileA.name = QStringLiteral("a.txt");
    fileA.size = 10;
    fileA.mtime = QDateTime::currentDateTimeUtc();
    fileA.fileType = QStringLiteral("text/plain");
    const int fileAId = db.upsertFile(fileA);
    QVERIFY(fileAId >= 0);

    FileInfo fileB = fileA;
    fileB.name = QStringLiteral("b.txt");
    const int fileBId = db.upsertFile(fileB);
    QVERIFY(fileBId >= 0);

    QVERIFY(db.addFileToVirtualFolder(folderId, fileAId));
    QVERIFY(db.addFileToVirtualFolder(folderId, fileBId));

    auto items = db.listVirtualFolderItems(folderId);
    QCOMPARE(items.size(), 2);

    // Duplicate add is idempotent (INSERT OR IGNORE)
    QVERIFY(db.addFileToVirtualFolder(folderId, fileAId));
    QCOMPARE(db.listVirtualFolderItems(folderId).size(), 2);

    // Remove file from folder
    QVERIFY(db.removeFileFromVirtualFolder(folderId, fileAId));
    QCOMPARE(db.listVirtualFolderItems(folderId).size(), 1);

    // Empty folder listing
    QVERIFY(db.listVirtualFolderItems(childId).isEmpty());

    // Delete virtual folder (cascade should remove child)
    QVERIFY(db.deleteVirtualFolder(folderId));
    QVERIFY(db.listVirtualFolders(-1).isEmpty());
    QVERIFY(!db.getVirtualFolder(folderId).has_value());

    // Empty name is rejected
    QCOMPARE(db.createVirtualFolder(QString(), -1), -1);
    QVERIFY(!db.renameVirtualFolder(folderId, QString()));
}

void KatalogueDatabaseTest::testProjectStatsAndListAllFiles() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());
    const QString dbPath = tmp.filePath("stats.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    // Empty catalog stats
    auto stats = db.projectStats();
    QVERIFY(stats.has_value());
    QCOMPARE(stats->volumeCount, 0);
    QCOMPARE(stats->fileCount, 0);
    QCOMPARE(stats->totalBytes, 0);

    // Empty listAllFiles
    QVERIFY(db.listAllFiles().isEmpty());

    // Create 2 volumes with files
    VolumeInfo vol1;
    vol1.label = QStringLiteral("Volume A");
    const int vol1Id = db.upsertVolume(vol1);
    QVERIFY(vol1Id >= 0);

    DirectoryInfo root1;
    root1.volumeId = vol1Id;
    root1.parentId = -1;
    root1.name = QStringLiteral("/");
    root1.fullPath = QStringLiteral("/");
    const int root1Id = db.upsertDirectory(root1);
    QVERIFY(root1Id >= 0);

    FileInfo f1;
    f1.directoryId = root1Id;
    f1.name = QStringLiteral("file1.txt");
    f1.size = 100;
    f1.mtime = QDateTime::currentDateTimeUtc();
    f1.fileType = QStringLiteral("text/plain");
    QVERIFY(db.upsertFile(f1) >= 0);

    FileInfo f2 = f1;
    f2.name = QStringLiteral("file2.txt");
    f2.size = 200;
    QVERIFY(db.upsertFile(f2) >= 0);

    VolumeInfo vol2;
    vol2.label = QStringLiteral("Volume B");
    const int vol2Id = db.upsertVolume(vol2);
    QVERIFY(vol2Id >= 0);

    DirectoryInfo root2;
    root2.volumeId = vol2Id;
    root2.parentId = -1;
    root2.name = QStringLiteral("/");
    root2.fullPath = QStringLiteral("/");
    const int root2Id = db.upsertDirectory(root2);
    QVERIFY(root2Id >= 0);

    FileInfo f3;
    f3.directoryId = root2Id;
    f3.name = QStringLiteral("file3.bin");
    f3.size = 500;
    f3.mtime = QDateTime::currentDateTimeUtc();
    f3.fileType = QStringLiteral("application/octet-stream");
    QVERIFY(db.upsertFile(f3) >= 0);

    // Verify stats
    stats = db.projectStats();
    QVERIFY(stats.has_value());
    QCOMPARE(stats->volumeCount, 2);
    QCOMPARE(stats->fileCount, 3);
    QCOMPARE(stats->totalBytes, 800);

    // listAllFiles without filter
    auto allFiles = db.listAllFiles();
    QCOMPARE(allFiles.size(), 3);

    // listAllFiles with volume filter
    auto vol1Files = db.listAllFiles(vol1Id);
    QCOMPARE(vol1Files.size(), 2);

    auto vol2Files = db.listAllFiles(vol2Id);
    QCOMPARE(vol2Files.size(), 1);
    QCOMPARE(vol2Files.first().fileName, QStringLiteral("file3.bin"));
}

QTEST_MAIN(KatalogueDatabaseTest)
#include "tst_katalogue_database.moc"
