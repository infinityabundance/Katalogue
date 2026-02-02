#pragma once

#include <QSqlDatabase>

#include "katalogue_types.h"

class KatalogueDatabase {
public:
    KatalogueDatabase();
    ~KatalogueDatabase();

    bool openProject(const QString &path);
    bool isOpen() const;

    int upsertVolume(const VolumeInfo &info);
    std::optional<VolumeInfo> findVolumeByFsUuid(const QString &fsUuid) const;
    bool clearVolumeContents(int volumeId);
    int upsertDirectory(const DirectoryInfo &info);
    int insertFile(const FileInfo &info);
    int upsertFile(const FileInfo &info);
    bool deleteFile(int fileId);

    QList<VolumeInfo> listVolumes() const;
    QList<DirectoryInfo> listDirectories(int volumeId, int parentId) const;
    QList<FileInfo> listFilesInDirectory(int directoryId) const;
    std::optional<DirectoryInfo> getDirectory(int directoryId) const;

    QList<SearchResult> searchByName(const QString &query,
                                     int limit = 100,
                                     int offset = 0) const;

private:
    bool initializeSchema();
    QString directoryFullPath(int directoryId) const;

    QSqlDatabase m_db;
    QString m_connectionName;
};
