#include <QtTest>

#include "katalogue_daemon.h"

class KatalogueDaemonTest : public QObject {
    Q_OBJECT
private slots:
    void testScanAndSearch();
    void testEdgeCases();
};

void KatalogueDaemonTest::testScanAndSearch() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString rootPath = tmp.path();
    QDir dir(rootPath);
    QVERIFY(dir.mkpath("docs"));

    QFile file(dir.filePath("docs/report.txt"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("hello");
    file.close();

    QTemporaryDir dbDir;
    QVERIFY(dbDir.isValid());
    const QString dbPath = dbDir.filePath("daemon.kdcatalog");

    KatalogueDaemon daemon;
    const auto info = daemon.OpenProject(dbPath);
    QVERIFY(info.value("ok").toBool());

    const uint scanId = daemon.StartScan(rootPath);
    QVERIFY(scanId > 0);

    QElapsedTimer timer;
    timer.start();
    QString status;
    while (timer.elapsed() < 10000) {
        const auto scan = daemon.GetScanStatus(scanId);
        status = scan.value("status").toString();
        if (status == QLatin1String("finished") || status == QLatin1String("failed")) {
            break;
        }
        QTest::qWait(50);
    }
    QCOMPARE(status, QStringLiteral("finished"));

    const auto volumes = daemon.ListVolumes();
    const auto volumeItems = volumes.value("items").toList();
    QVERIFY(!volumeItems.isEmpty());

    const auto results = daemon.Search(QStringLiteral("report"), -1, QString(), 50, 0);
    QVERIFY(!results.isEmpty());
}

void KatalogueDaemonTest::testEdgeCases() {
    QTemporaryDir dbDir;
    QVERIFY(dbDir.isValid());
    const QString dbPath = dbDir.filePath("edge.kdcatalog");

    KatalogueDaemon daemon;
    const auto info = daemon.OpenProject(dbPath);
    QVERIFY(info.value("ok").toBool());

    // Empty path should fail gracefully
    const auto emptyInfo = daemon.OpenProject(QString());
    QVERIFY(!emptyInfo.value("ok").toBool());

    // GetScanStatus with invalid ID returns "unknown"
    const auto badStatus = daemon.GetScanStatus(99999);
    QCOMPARE(badStatus.value("status").toString(), QStringLiteral("unknown"));

    // Search on empty catalog returns empty results
    const auto emptyResults = daemon.Search(QStringLiteral("anything"), -1, QString(), 50, 0);
    QVERIFY(emptyResults.isEmpty());

    // ListVolumes on empty catalog returns empty items
    const auto emptyVolumes = daemon.ListVolumes();
    QVERIFY(emptyVolumes.value("items").toList().isEmpty());

    // ListDirectories on non-existent volume returns empty
    const auto emptyDirs = daemon.ListDirectories(999, -1);
    QVERIFY(emptyDirs.isEmpty());

    // ListFiles on non-existent directory returns empty
    const auto emptyFiles = daemon.ListFiles(999);
    QVERIFY(emptyFiles.isEmpty());

    // Virtual folder operations on empty catalog
    const auto emptyFolders = daemon.ListVirtualFolders(-1);
    QVERIFY(emptyFolders.isEmpty());

    // Create and manipulate virtual folder via daemon
    const int folderId = daemon.CreateVirtualFolder(QStringLiteral("Test VF"), -1);
    QVERIFY(folderId >= 0);
    auto folders = daemon.ListVirtualFolders(-1);
    QCOMPARE(folders.size(), 1);

    daemon.RenameVirtualFolder(folderId, QStringLiteral("Renamed VF"));
    folders = daemon.ListVirtualFolders(-1);
    QCOMPARE(folders.first().value(QStringLiteral("name")).toString(), QStringLiteral("Renamed VF"));

    daemon.DeleteVirtualFolder(folderId);
    QVERIFY(daemon.ListVirtualFolders(-1).isEmpty());

    // File note on non-existent file returns empty
    QVERIFY(daemon.GetFileNote(999).isEmpty());

    // File tags on non-existent file returns empty
    QVERIFY(daemon.GetFileTags(999).isEmpty());
}

QTEST_MAIN(KatalogueDaemonTest)
#include "tst_katalogue_daemon.moc"
