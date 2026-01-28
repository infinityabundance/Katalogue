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
    int upsertDirectory(const DirectoryInfo &info);
    int insertFile(const FileInfo &info);

    QList<VolumeInfo> listVolumes() const;

    QList<SearchResult> searchByName(const QString &query,
                                     int limit = 100,
                                     int offset = 0) const;

private:
    bool initializeSchema();
    QString directoryFullPath(int directoryId) const;

    QSqlDatabase m_db;
    QString m_connectionName;
};
