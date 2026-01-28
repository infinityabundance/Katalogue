#include "katalogue_database.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

KatalogueDatabase::KatalogueDatabase() = default;

KatalogueDatabase::~KatalogueDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    if (!m_connectionName.isEmpty()) {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool KatalogueDatabase::openProject(const QString &path) {
    if (m_db.isOpen()) {
        m_db.close();
    }

    if (m_connectionName.isEmpty()) {
        m_connectionName = QStringLiteral("katalogue_core_%1")
                               .arg(reinterpret_cast<quintptr>(this));
    }

    if (!QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    } else {
        m_db = QSqlDatabase::database(m_connectionName);
    }

    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        qWarning() << "Failed to open database" << m_db.lastError();
        return false;
    }

    return initializeSchema();
}

bool KatalogueDatabase::isOpen() const {
    return m_db.isOpen();
}

bool KatalogueDatabase::initializeSchema() {
    if (!m_db.isOpen()) {
        return false;
    }

    if (!m_db.transaction()) {
        qWarning() << "Failed to start transaction" << m_db.lastError();
        return false;
    }

    QSqlQuery query(m_db);
    const char *schemaStatements[] = {
        "CREATE TABLE IF NOT EXISTS volumes ("
        "id INTEGER PRIMARY KEY,"
        "label TEXT NOT NULL,"
        "description TEXT,"
        "fs_uuid TEXT,"
        "fs_type TEXT,"
        "physical_hint TEXT,"
        "total_size INTEGER,"
        "created_at INTEGER,"
        "updated_at INTEGER"
        ");",
        "CREATE TABLE IF NOT EXISTS directories ("
        "id INTEGER PRIMARY KEY,"
        "volume_id INTEGER NOT NULL REFERENCES volumes(id),"
        "parent_id INTEGER REFERENCES directories(id),"
        "name TEXT NOT NULL,"
        "full_path TEXT NOT NULL,"
        "UNIQUE(volume_id, full_path)"
        ");",
        "CREATE TABLE IF NOT EXISTS files ("
        "id INTEGER PRIMARY KEY,"
        "directory_id INTEGER NOT NULL REFERENCES directories(id),"
        "name TEXT NOT NULL,"
        "size INTEGER,"
        "mtime INTEGER,"
        "ctime INTEGER,"
        "file_type TEXT,"
        "hash TEXT,"
        "attrs INTEGER"
        ");",
        "CREATE VIRTUAL TABLE IF NOT EXISTS file_fts USING fts5("
        "name,"
        "full_path,"
        "tokenize='porter'"
        ");"};

    for (const char *statement : schemaStatements) {
        if (!query.exec(QString::fromUtf8(statement))) {
            qWarning() << "Failed to apply schema" << query.lastError();
            m_db.rollback();
            return false;
        }
    }

    if (!m_db.commit()) {
        qWarning() << "Failed to commit schema" << m_db.lastError();
        return false;
    }

    return true;
}

int KatalogueDatabase::upsertVolume(const VolumeInfo &info) {
    if (!m_db.isOpen()) {
        return -1;
    }

    if (info.id >= 0) {
        QSqlQuery update(m_db);
        update.prepare("UPDATE volumes SET label = ?, description = ?, fs_uuid = ?, fs_type = ?, "
                       "physical_hint = ?, total_size = ?, created_at = ?, updated_at = ? WHERE id = ?");
        update.addBindValue(info.label);
        update.addBindValue(info.description);
        update.addBindValue(info.fsUuid);
        update.addBindValue(info.fsType);
        update.addBindValue(info.physicalHint);
        update.addBindValue(info.totalSize);
        update.addBindValue(info.createdAt.isValid() ? info.createdAt.toSecsSinceEpoch()
                                                    : QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
        update.addBindValue(info.updatedAt.isValid() ? info.updatedAt.toSecsSinceEpoch()
                                                    : QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
        update.addBindValue(info.id);
        if (!update.exec()) {
            qWarning() << "Failed to update volume" << update.lastError();
            return -1;
        }
        return info.id;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO volumes (label, description, fs_uuid, fs_type, physical_hint, total_size, "
                   "created_at, updated_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    insert.addBindValue(info.label);
    insert.addBindValue(info.description);
    insert.addBindValue(info.fsUuid);
    insert.addBindValue(info.fsType);
    insert.addBindValue(info.physicalHint);
    insert.addBindValue(info.totalSize);
    insert.addBindValue(info.createdAt.isValid() ? info.createdAt.toSecsSinceEpoch()
                                                : QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
    insert.addBindValue(info.updatedAt.isValid() ? info.updatedAt.toSecsSinceEpoch()
                                                : QDateTime::currentDateTimeUtc().toSecsSinceEpoch());

    if (!insert.exec()) {
        qWarning() << "Failed to insert volume" << insert.lastError();
        return -1;
    }

    return insert.lastInsertId().toInt();
}

int KatalogueDatabase::upsertDirectory(const DirectoryInfo &info) {
    if (!m_db.isOpen()) {
        return -1;
    }

    if (info.id >= 0) {
        QSqlQuery update(m_db);
        update.prepare("UPDATE directories SET volume_id = ?, parent_id = ?, name = ?, full_path = ? "
                       "WHERE id = ?");
        update.addBindValue(info.volumeId);
        update.addBindValue(info.parentId >= 0 ? QVariant(info.parentId) : QVariant(QVariant::Int));
        update.addBindValue(info.name);
        update.addBindValue(info.fullPath);
        update.addBindValue(info.id);
        if (!update.exec()) {
            qWarning() << "Failed to update directory" << update.lastError();
            return -1;
        }
        return info.id;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO directories (volume_id, parent_id, name, full_path) VALUES (?, ?, ?, ?)");
    insert.addBindValue(info.volumeId);
    insert.addBindValue(info.parentId >= 0 ? QVariant(info.parentId) : QVariant(QVariant::Int));
    insert.addBindValue(info.name);
    insert.addBindValue(info.fullPath);

    if (!insert.exec()) {
        if (insert.lastError().isValid()) {
            QSqlQuery select(m_db);
            select.prepare("SELECT id FROM directories WHERE volume_id = ? AND full_path = ?");
            select.addBindValue(info.volumeId);
            select.addBindValue(info.fullPath);
            if (select.exec() && select.next()) {
                return select.value(0).toInt();
            }
        }
        qWarning() << "Failed to insert directory" << insert.lastError();
        return -1;
    }

    return insert.lastInsertId().toInt();
}

int KatalogueDatabase::insertFile(const FileInfo &info) {
    if (!m_db.isOpen()) {
        return -1;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO files (directory_id, name, size, mtime, ctime, file_type, hash, attrs) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    insert.addBindValue(info.directoryId);
    insert.addBindValue(info.name);
    insert.addBindValue(info.size);
    insert.addBindValue(info.mtime.isValid() ? info.mtime.toSecsSinceEpoch() : QVariant(QVariant::LongLong));
    insert.addBindValue(info.ctime.isValid() ? info.ctime.toSecsSinceEpoch() : QVariant(QVariant::LongLong));
    insert.addBindValue(info.fileType);
    insert.addBindValue(info.hash);
    insert.addBindValue(info.attrs);

    if (!insert.exec()) {
        qWarning() << "Failed to insert file" << insert.lastError();
        return -1;
    }

    int fileId = insert.lastInsertId().toInt();
    const QString fullPath = directoryFullPath(info.directoryId) + '/' + info.name;

    QSqlQuery fts(m_db);
    fts.prepare("INSERT INTO file_fts (rowid, name, full_path) VALUES (?, ?, ?)");
    fts.addBindValue(fileId);
    fts.addBindValue(info.name);
    fts.addBindValue(fullPath);

    if (!fts.exec()) {
        qWarning() << "Failed to insert FTS row" << fts.lastError();
    }

    return fileId;
}

QList<VolumeInfo> KatalogueDatabase::listVolumes() const {
    QList<VolumeInfo> volumes;
    if (!m_db.isOpen()) {
        return volumes;
    }

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, label, description, fs_uuid, fs_type, physical_hint, total_size, "
                    "created_at, updated_at FROM volumes")) {
        qWarning() << "Failed to list volumes" << query.lastError();
        return volumes;
    }

    while (query.next()) {
        VolumeInfo info;
        info.id = query.value(0).toInt();
        info.label = query.value(1).toString();
        info.description = query.value(2).toString();
        info.fsUuid = query.value(3).toString();
        info.fsType = query.value(4).toString();
        info.physicalHint = query.value(5).toString();
        info.totalSize = query.value(6).toLongLong();
        info.createdAt = QDateTime::fromSecsSinceEpoch(query.value(7).toLongLong(), Qt::UTC);
        info.updatedAt = QDateTime::fromSecsSinceEpoch(query.value(8).toLongLong(), Qt::UTC);
        volumes.append(info);
    }

    return volumes;
}

QList<SearchResult> KatalogueDatabase::searchByName(const QString &queryText, int limit, int offset) const {
    QList<SearchResult> results;
    if (!m_db.isOpen()) {
        return results;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT files.id, files.name, directories.full_path || '/' || files.name AS full_path, "
                  "volumes.label, files.size, files.mtime "
                  "FROM file_fts "
                  "JOIN files ON files.id = file_fts.rowid "
                  "JOIN directories ON directories.id = files.directory_id "
                  "JOIN volumes ON volumes.id = directories.volume_id "
                  "WHERE file_fts MATCH ? "
                  "LIMIT ? OFFSET ?");
    const QString matchQuery = queryText + '*';
    query.addBindValue(matchQuery);
    query.addBindValue(limit);
    query.addBindValue(offset);

    if (!query.exec()) {
        qWarning() << "Failed to search" << query.lastError();
        return results;
    }

    while (query.next()) {
        SearchResult result;
        result.fileId = query.value(0).toInt();
        result.fileName = query.value(1).toString();
        result.fullPath = query.value(2).toString();
        result.volumeLabel = query.value(3).toString();
        result.size = query.value(4).toLongLong();
        result.mtime = QDateTime::fromSecsSinceEpoch(query.value(5).toLongLong(), Qt::UTC);
        results.append(result);
    }

    return results;
}

QString KatalogueDatabase::directoryFullPath(int directoryId) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT full_path FROM directories WHERE id = ?");
    query.addBindValue(directoryId);
    if (!query.exec()) {
        qWarning() << "Failed to fetch directory path" << query.lastError();
        return {};
    }
    if (query.next()) {
        return query.value(0).toString();
    }
    return {};
}
