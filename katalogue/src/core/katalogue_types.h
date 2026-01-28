#pragma once

#include <QDateTime>
#include <QString>

struct VolumeInfo {
    int id = -1;
    QString label;
    QString description;
    QString fsUuid;
    QString fsType;
    QString physicalHint;
    qint64 totalSize = 0;
    QDateTime createdAt;
    QDateTime updatedAt;
};

struct DirectoryInfo {
    int id = -1;
    int volumeId = -1;
    int parentId = -1;
    QString name;
    QString fullPath;
};

struct FileInfo {
    int id = -1;
    int directoryId = -1;
    QString name;
    qint64 size = 0;
    QDateTime mtime;
    QDateTime ctime;
    QString fileType;
    QString hash;
    quint32 attrs = 0;
};

struct SearchResult {
    int fileId;
    QString fileName;
    QString fullPath;
    QString volumeLabel;
    qint64 size;
    QDateTime mtime;
};
