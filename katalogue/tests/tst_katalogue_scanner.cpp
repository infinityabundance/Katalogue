#include <QtTest>

#include "katalogue_database.h"
#include "katalogue_scanner.h"

class KatalogueScannerTest : public QObject {
    Q_OBJECT
private slots:
    void testScanTree();
    void testMaxDepth();
};

void KatalogueScannerTest::testScanTree() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString rootPath = tmp.path();
    QDir dir(rootPath);
    QVERIFY(dir.mkpath("sub1"));
    QVERIFY(dir.mkpath("sub2"));

    QFile file1(dir.filePath("file1.txt"));
    QVERIFY(file1.open(QIODevice::WriteOnly));
    file1.write("hello");
    file1.close();

    QFile hidden(dir.filePath("sub1/.hidden"));
    QVERIFY(hidden.open(QIODevice::WriteOnly));
    hidden.write("secret");
    hidden.close();

    QFile file2(dir.filePath("sub2/file2.bin"));
    QVERIFY(file2.open(QIODevice::WriteOnly));
    file2.write("data");
    file2.close();

    QTemporaryDir dbDir;
    QVERIFY(dbDir.isValid());
    const QString dbPath = dbDir.filePath("scan.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    KatalogueScanner scanner;
    ScanOptions options;
    options.includeHidden = false;

    QVERIFY(scanner.scan(rootPath, db, {}, options));

    const auto volumes = db.listVolumes();
    QCOMPARE(volumes.size(), 1);

    const int volumeId = volumes.first().id;
    const auto roots = db.listDirectories(volumeId, -1);
    QVERIFY(!roots.isEmpty());

    const auto results = db.searchByName(QStringLiteral("file"));
    QVERIFY(!results.isEmpty());

    const auto hiddenResults = db.searchByName(QStringLiteral("hidden"));
    QVERIFY(hiddenResults.isEmpty());
}

void KatalogueScannerTest::testMaxDepth() {
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString rootPath = tmp.path();
    QDir dir(rootPath);
    QVERIFY(dir.mkpath("sub"));
    QFile file(dir.filePath("sub/file.txt"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("data");
    file.close();

    QTemporaryDir dbDir;
    QVERIFY(dbDir.isValid());
    const QString dbPath = dbDir.filePath("depth.kdcatalog");

    KatalogueDatabase db;
    QVERIFY(db.openProject(dbPath));

    KatalogueScanner scanner;
    ScanOptions options;
    options.maxDepth = 0;

    QVERIFY(scanner.scan(rootPath, db, {}, options));

    const auto results = db.searchByName(QStringLiteral("file"));
    QVERIFY(results.isEmpty());
}

QTEST_MAIN(KatalogueScannerTest)
#include "tst_katalogue_scanner.moc"
