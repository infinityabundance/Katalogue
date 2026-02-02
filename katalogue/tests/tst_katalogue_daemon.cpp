#include <QtTest>

#include "katalogue_daemon.h"

class KatalogueDaemonTest : public QObject {
    Q_OBJECT
private slots:
    void testScanAndSearch();
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

QTEST_MAIN(KatalogueDaemonTest)
#include "tst_katalogue_daemon.moc"
