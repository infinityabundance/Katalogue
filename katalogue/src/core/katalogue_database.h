#pragma once

#include <QSqlDatabase>

#include "katalogue_types.h"

class KatalogueDatabase {
public:
    KatalogueDatabase();
    ~KatalogueDatabase();

    struct ProjectStats {
        int volumeCount = 0;
        qint64 fileCount = 0;
        qint64 totalBytes = 0;
    };

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
    std::optional<ProjectStats> projectStats() const;
    QList<DirectoryInfo> listDirectories(int volumeId, int parentId) const;
    QList<FileInfo> listFilesInDirectory(int directoryId) const;
    std::optional<DirectoryInfo> getDirectory(int directoryId) const;
    std::optional<QString> getVolumeLabel(int volumeId) const;

    struct SearchFilters {
        std::optional<int> volumeId;
        std::optional<QString> fileType;
    };

    QList<SearchResult> search(const QString &queryText,
                               const SearchFilters &filters,
                               int limit,
                               int offset) const;

    QList<SearchResult> searchByName(const QString &query,
                                     int limit = 100,
                                     int offset = 0) const;

    std::optional<QString> getNoteForFile(int fileId) const;
    std::optional<QString> getNoteForDirectory(int directoryId) const;
    bool setNoteForFile(int fileId, const QString &content);
    bool setNoteForDirectory(int directoryId, const QString &content);

    bool addTagToFile(int fileId, const QString &key, const QString &value);
    bool removeTagFromFile(int fileId, const QString &key, const QString &value);
    QList<QPair<QString, QString>> tagsForFile(int fileId) const;

    int createVirtualFolder(const QString &name, int parentId = -1);
    bool renameVirtualFolder(int folderId, const QString &newName);
    bool deleteVirtualFolder(int folderId);
    QList<VirtualFolderInfo> listVirtualFolders(int parentId) const;
    std::optional<VirtualFolderInfo> getVirtualFolder(int folderId) const;

    bool addFileToVirtualFolder(int folderId, int fileId);
    bool removeFileFromVirtualFolder(int folderId, int fileId);
    QList<SearchResult> listVirtualFolderItems(int folderId) const;

private:
    bool initializeSchema();
    QString directoryFullPath(int directoryId) const;

    QSqlDatabase m_db;
    QString m_connectionName;
};
