#pragma once

#include <memory>

#include <QObject>
#include <QThread>
#include <QDBusContext>

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
    VolumeInfo volumeInfo;
    ScanOptions options;
    bool existingVolume = false;
    QString errorString;
};

class KatalogueDaemon : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Katalogue1")
public:
    explicit KatalogueDaemon(QObject *parent = nullptr);
    bool init();

public slots:
    QString Ping() const;
    QVariantMap OpenProject(const QString &path);
    QVariantMap GetProjectInfo() const;
    uint StartScan(const QString &rootPath);
    bool CancelScan(uint scanId);
    QVariantMap GetScanStatus(uint scanId) const;
    QVariantMap ListVolumes() const;
    QList<QVariantMap> ListDirectories(int volumeId, int parentId) const;
    QList<QVariantMap> ListFiles(int directoryId) const;
    QVariantMap SearchByName(const QString &query, int limit, int offset) const;
    QList<QVariantMap> Search(const QString &query, int volumeId, const QString &fileType, int limit, int offset) const;
    QString GetFileNote(int fileId) const;
    void SetFileNote(int fileId, const QString &content);
    QList<QVariantMap> GetFileTags(int fileId) const;
    void AddFileTag(int fileId, const QString &key, const QString &value);
    void RemoveFileTag(int fileId, const QString &key, const QString &value);
    QList<QVariantMap> ListVirtualFolders(int parentId) const;
    int CreateVirtualFolder(const QString &name, int parentId);
    void RenameVirtualFolder(int folderId, const QString &newName);
    void DeleteVirtualFolder(int folderId);
    QList<QVariantMap> ListVirtualFolderItems(int folderId) const;
    void AddFileToVirtualFolder(int folderId, int fileId);
    void RemoveFileFromVirtualFolder(int folderId, int fileId);
    void RenameVolume(int volumeId, const QString &newLabel);

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
