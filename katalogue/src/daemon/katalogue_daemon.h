#pragma once

#include <memory>

#include <QObject>
#include <QThread>

#include "katalogue_database.h"
#include "katalogue_scanner.h"

struct ScanJob {
    enum class Status {
        Pending,
        Running,
        Finished,
        Cancelled,
        Failed
    };

    uint id = 0;
    Status status = Status::Pending;
    QString rootPath;
    ScanStats stats;
};

class KatalogueDaemon : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Katalogue1")
public:
    explicit KatalogueDaemon(QObject *parent = nullptr);

public slots:
    QString Ping() const;
    bool OpenProject(const QString &path);
    uint StartScan(const QString &rootPath);
    bool CancelScan(uint scanId);
    QVariantMap GetScanStatus(uint scanId) const;
    QVariantMap ListVolumes() const;
    QList<QVariantMap> ListDirectories(int volumeId, int parentId) const;
    QList<QVariantMap> ListFiles(int directoryId) const;
    QVariantMap SearchByName(const QString &query, int limit, int offset) const;

signals:
    void ScanProgress(uint scanId, const QString &path, int directories, int files, qint64 bytes);
    void ScanFinished(uint scanId, const QString &status);

private:
    void runScan(uint scanId);
    QString statusToString(ScanJob::Status status) const;

    KatalogueDatabase m_db;
    KatalogueScanner m_scanner;
    QThread m_scanThread;
    QHash<uint, ScanJob> m_jobs;
    uint m_nextScanId = 1;
    QString m_projectPath;
};
