#include "katalogue_database.h"

#include <QDateTime>
#include <QDir>
#include <QList>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

namespace {
constexpr int CURRENT_SCHEMA_VERSION = 3;

bool execStatements(QSqlDatabase &db, const QList<QString> &statements) {
    QSqlQuery query(db);
    for (const auto &statement : statements) {
        if (!query.exec(statement)) {
            qWarning() << "Failed to apply schema" << query.lastError();
            return false;
        }
    }
    return true;
}

int currentSchemaVersion(QSqlDatabase &db) {
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("PRAGMA user_version"))) {
        qWarning() << "Failed to read schema version" << query.lastError();
        return -1;
    }
    return query.next() ? query.value(0).toInt() : -1;
}

bool setSchemaVersion(QSqlDatabase &db, int version) {
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("PRAGMA user_version = %1").arg(version))) {
        qWarning() << "Failed to set schema version" << query.lastError();
        return false;
    }
    return true;
}

bool tableExists(QSqlDatabase &db, const QString &name) {
    QSqlQuery query(db);
    query.prepare("SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?");
    query.addBindValue(name);
    if (!query.exec()) {
        return false;
    }
    return query.next();
}

bool setSchemaInfoVersion(QSqlDatabase &db, int version) {
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS schema_info ("
            "id INTEGER PRIMARY KEY CHECK (id = 1),"
            "version INTEGER NOT NULL"
            ")"))) {
        qWarning() << "Failed to create schema_info" << query.lastError();
        return false;
    }
    query.prepare("INSERT OR REPLACE INTO schema_info (id, version) VALUES (1, ?)");
    query.addBindValue(version);
    if (!query.exec()) {
        qWarning() << "Failed to set schema_info version" << query.lastError();
        return false;
    }
    return true;
}
} // namespace

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
    m_lastErrorString.clear();

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
        m_lastErrorString = m_db.lastError().text();
        return false;
    }

    QSqlQuery pragma(m_db);
    if (!pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"))) {
        qWarning() << "Failed to enable foreign keys" << pragma.lastError();
    }

    if (!tableExists(m_db, QStringLiteral("schema_info"))) {
        if (!initializeSchema()) {
            m_lastErrorString = QStringLiteral("Failed to initialize schema");
            m_db.close();
            return false;
        }
    }

    const auto status = checkSchema();
    if (status != SchemaStatus::Ok) {
        if (m_lastErrorString.isEmpty()) {
            m_lastErrorString = QStringLiteral("Schema check failed");
        }
        m_db.close();
        return false;
    }

    return true;
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

    int version = currentSchemaVersion(m_db);
    if (version < 0) {
        m_db.rollback();
        return false;
    }

    if (version == 0) {
        const QList<QString> schemaStatements = {
            QStringLiteral(
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
                ");"),
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS directories ("
                "id INTEGER PRIMARY KEY,"
                "volume_id INTEGER NOT NULL REFERENCES volumes(id) ON DELETE CASCADE,"
                "parent_id INTEGER REFERENCES directories(id) ON DELETE CASCADE,"
                "name TEXT NOT NULL,"
                "full_path TEXT NOT NULL,"
                "UNIQUE(volume_id, full_path)"
                ");"),
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS files ("
                "id INTEGER PRIMARY KEY,"
                "directory_id INTEGER NOT NULL REFERENCES directories(id) ON DELETE CASCADE,"
                "name TEXT NOT NULL,"
                "size INTEGER,"
                "mtime INTEGER,"
                "ctime INTEGER,"
                "file_type TEXT,"
                "hash TEXT,"
                "attrs INTEGER,"
                "UNIQUE(directory_id, name)"
                ");"),
            QStringLiteral(
                "CREATE VIRTUAL TABLE IF NOT EXISTS file_fts USING fts5("
                "name,"
                "full_path,"
                "tokenize='porter'"
                ");"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS directories_volume_idx ON directories(volume_id);"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS directories_parent_idx ON directories(parent_id);"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS files_directory_idx ON files(directory_id);"),
            QStringLiteral(
                "CREATE UNIQUE INDEX IF NOT EXISTS volumes_fs_uuid_idx ON volumes(fs_uuid) WHERE fs_uuid IS NOT NULL;"),
            QStringLiteral(
                "CREATE TRIGGER IF NOT EXISTS files_ai AFTER INSERT ON files BEGIN "
                "INSERT INTO file_fts(rowid, name, full_path) "
                "VALUES (new.id, new.name, "
                "(SELECT full_path || '/' || new.name FROM directories WHERE id = new.directory_id)); "
                "END;"),
            QStringLiteral(
                "CREATE TRIGGER IF NOT EXISTS files_au AFTER UPDATE OF name, directory_id ON files BEGIN "
                "DELETE FROM file_fts WHERE rowid = old.id; "
                "INSERT INTO file_fts(rowid, name, full_path) "
                "VALUES (new.id, new.name, "
                "(SELECT full_path || '/' || new.name FROM directories WHERE id = new.directory_id)); "
                "END;"),
            QStringLiteral(
                "CREATE TRIGGER IF NOT EXISTS files_ad AFTER DELETE ON files BEGIN "
                "DELETE FROM file_fts WHERE rowid = old.id; "
                "END;"),
            QStringLiteral(
                "CREATE TRIGGER IF NOT EXISTS directories_au AFTER UPDATE OF full_path ON directories BEGIN "
                "UPDATE file_fts SET full_path = "
                "(SELECT new.full_path || '/' || files.name FROM files WHERE files.id = file_fts.rowid) "
                "WHERE rowid IN (SELECT id FROM files WHERE directory_id = new.id); "
                "END;"),
            QStringLiteral(
                "INSERT INTO file_fts(rowid, name, full_path) "
                "SELECT files.id, files.name, directories.full_path || '/' || files.name "
                "FROM files JOIN directories ON directories.id = files.directory_id "
                "WHERE files.id NOT IN (SELECT rowid FROM file_fts);")
        };

        if (!execStatements(m_db, schemaStatements)) {
            m_db.rollback();
            return false;
        }

        if (!setSchemaVersion(m_db, 1)) {
            m_db.rollback();
            return false;
        }
        version = 1;
    }

    if (version == 1) {
        const QList<QString> schemaStatements = {
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS notes ("
                "id INTEGER PRIMARY KEY,"
                "target_type TEXT NOT NULL,"
                "target_id INTEGER NOT NULL,"
                "content TEXT NOT NULL"
                ");"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS notes_target_idx "
                "ON notes(target_type, target_id);"),
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS tags ("
                "id INTEGER PRIMARY KEY,"
                "key TEXT NOT NULL,"
                "value TEXT"
                ");"),
            QStringLiteral(
                "CREATE UNIQUE INDEX IF NOT EXISTS tags_unique_idx "
                "ON tags(key, value);"),
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS file_tags ("
                "file_id INTEGER NOT NULL REFERENCES files(id) ON DELETE CASCADE,"
                "tag_id INTEGER NOT NULL REFERENCES tags(id) ON DELETE CASCADE,"
                "PRIMARY KEY(file_id, tag_id)"
                ");")
        };

        if (!execStatements(m_db, schemaStatements)) {
            m_db.rollback();
            return false;
        }

        if (!setSchemaVersion(m_db, 2)) {
            m_db.rollback();
            return false;
        }
        version = 2;
    }

    if (version == 2) {
        const QList<QString> schemaStatements = {
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS virtual_folders ("
                "id INTEGER PRIMARY KEY,"
                "parent_id INTEGER REFERENCES virtual_folders(id) ON DELETE CASCADE,"
                "name TEXT NOT NULL"
                ");"),
            QStringLiteral(
                "CREATE TABLE IF NOT EXISTS virtual_folder_items ("
                "folder_id INTEGER NOT NULL REFERENCES virtual_folders(id) ON DELETE CASCADE,"
                "file_id INTEGER NOT NULL REFERENCES files(id) ON DELETE CASCADE,"
                "PRIMARY KEY(folder_id, file_id)"
                ");"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS virtual_folders_parent_idx "
                "ON virtual_folders(parent_id);"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS virtual_folder_items_folder_idx "
                "ON virtual_folder_items(folder_id);"),
            QStringLiteral(
                "CREATE INDEX IF NOT EXISTS virtual_folder_items_file_idx "
                "ON virtual_folder_items(file_id);")
        };

        if (!execStatements(m_db, schemaStatements)) {
            m_db.rollback();
            return false;
        }

        if (!setSchemaVersion(m_db, 3)) {
            m_db.rollback();
            return false;
        }
        version = 3;
    }

    if (!setSchemaInfoVersion(m_db, CURRENT_SCHEMA_VERSION)) {
        m_db.rollback();
        return false;
    }

    if (!setSchemaVersion(m_db, CURRENT_SCHEMA_VERSION)) {
        m_db.rollback();
        return false;
    }

    if (!m_db.commit()) {
        qWarning() << "Failed to commit schema" << m_db.lastError();
        return false;
    }

    return true;
}

KatalogueDatabase::SchemaStatus KatalogueDatabase::checkSchema() const {
    if (!m_db.isOpen()) {
        m_lastErrorString = QStringLiteral("Database not open");
        return SchemaStatus::Missing;
    }

    if (!tableExists(m_db, QStringLiteral("schema_info"))) {
        m_lastErrorString = QStringLiteral("schema_info table missing");
        return SchemaStatus::Missing;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT version FROM schema_info WHERE id = 1");
    if (!query.exec()) {
        m_lastErrorString = QStringLiteral("Failed to read schema_info");
        return SchemaStatus::Corrupt;
    }
    if (!query.next()) {
        m_lastErrorString = QStringLiteral("schema_info row missing");
        return SchemaStatus::Missing;
    }

    const int version = query.value(0).toInt();
    if (version == CURRENT_SCHEMA_VERSION) {
        return SchemaStatus::Ok;
    }
    if (version > CURRENT_SCHEMA_VERSION) {
        m_lastErrorString = QStringLiteral("Catalog was created with a newer Katalogue version");
        return SchemaStatus::Incompatible;
    }
    m_lastErrorString = QStringLiteral("Catalog schema is older than this version");
    return SchemaStatus::Incompatible;
}

int KatalogueDatabase::schemaVersion() const {
    if (!m_db.isOpen()) {
        return -1;
    }
    QSqlQuery query(m_db);
    query.prepare("SELECT version FROM schema_info WHERE id = 1");
    if (!query.exec()) {
        return -1;
    }
    if (!query.next()) {
        return -1;
    }
    return query.value(0).toInt();
}

QString KatalogueDatabase::lastErrorString() const {
    return m_lastErrorString;
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

std::optional<VolumeInfo> KatalogueDatabase::findVolumeByFsUuid(const QString &fsUuid) const {
    if (!m_db.isOpen() || fsUuid.trimmed().isEmpty()) {
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT id, label, description, fs_uuid, fs_type, physical_hint, total_size, "
                  "created_at, updated_at FROM volumes WHERE fs_uuid = ? LIMIT 1");
    query.addBindValue(fsUuid);
    if (!query.exec()) {
        qWarning() << "Failed to find volume by fs_uuid" << query.lastError();
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }

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
    return info;
}

bool KatalogueDatabase::clearVolumeContents(int volumeId) {
    if (!m_db.isOpen() || volumeId < 0) {
        return false;
    }

    if (!m_db.transaction()) {
        qWarning() << "Failed to start clear volume transaction" << m_db.lastError();
        return false;
    }

    QSqlQuery deleteFiles(m_db);
    deleteFiles.prepare("DELETE FROM files WHERE directory_id IN "
                        "(SELECT id FROM directories WHERE volume_id = ?)");
    deleteFiles.addBindValue(volumeId);
    if (!deleteFiles.exec()) {
        qWarning() << "Failed to clear files for volume" << deleteFiles.lastError();
        m_db.rollback();
        return false;
    }

    QSqlQuery deleteDirs(m_db);
    deleteDirs.prepare("DELETE FROM directories WHERE volume_id = ?");
    deleteDirs.addBindValue(volumeId);
    if (!deleteDirs.exec()) {
        qWarning() << "Failed to clear directories for volume" << deleteDirs.lastError();
        m_db.rollback();
        return false;
    }

    if (!m_db.commit()) {
        qWarning() << "Failed to commit clear volume" << m_db.lastError();
        return false;
    }

    return true;
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
    return upsertFile(info);
}

int KatalogueDatabase::upsertFile(const FileInfo &info) {
    if (!m_db.isOpen()) {
        return -1;
    }

    if (info.id >= 0) {
        QSqlQuery update(m_db);
        update.prepare("UPDATE files SET directory_id = ?, name = ?, size = ?, mtime = ?, ctime = ?, "
                       "file_type = ?, hash = ?, attrs = ? WHERE id = ?");
        update.addBindValue(info.directoryId);
        update.addBindValue(info.name);
        update.addBindValue(info.size);
        update.addBindValue(info.mtime.isValid() ? info.mtime.toSecsSinceEpoch() : QVariant(QVariant::LongLong));
        update.addBindValue(info.ctime.isValid() ? info.ctime.toSecsSinceEpoch() : QVariant(QVariant::LongLong));
        update.addBindValue(info.fileType);
        update.addBindValue(info.hash);
        update.addBindValue(info.attrs);
        update.addBindValue(info.id);
        if (!update.exec()) {
            qWarning() << "Failed to update file" << update.lastError();
            return -1;
        }
        return info.id;
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
        if (insert.lastError().isValid()) {
            QSqlQuery update(m_db);
            update.prepare("UPDATE files SET size = ?, mtime = ?, ctime = ?, file_type = ?, hash = ?, attrs = ? "
                           "WHERE directory_id = ? AND name = ?");
            update.addBindValue(info.size);
            update.addBindValue(info.mtime.isValid() ? info.mtime.toSecsSinceEpoch() : QVariant(QVariant::LongLong));
            update.addBindValue(info.ctime.isValid() ? info.ctime.toSecsSinceEpoch() : QVariant(QVariant::LongLong));
            update.addBindValue(info.fileType);
            update.addBindValue(info.hash);
            update.addBindValue(info.attrs);
            update.addBindValue(info.directoryId);
            update.addBindValue(info.name);
            if (!update.exec()) {
                qWarning() << "Failed to update existing file" << update.lastError();
                return -1;
            }

            QSqlQuery select(m_db);
            select.prepare("SELECT id FROM files WHERE directory_id = ? AND name = ?");
            select.addBindValue(info.directoryId);
            select.addBindValue(info.name);
            if (select.exec() && select.next()) {
                return select.value(0).toInt();
            }
        }
        qWarning() << "Failed to insert file" << insert.lastError();
        return -1;
    }

    return insert.lastInsertId().toInt();
}

bool KatalogueDatabase::deleteFile(int fileId) {
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM files WHERE id = ?");
    query.addBindValue(fileId);
    if (!query.exec()) {
        qWarning() << "Failed to delete file" << query.lastError();
        return false;
    }
    return true;
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

std::optional<KatalogueDatabase::ProjectStats> KatalogueDatabase::projectStats() const {
    if (!m_db.isOpen()) {
        return std::nullopt;
    }

    ProjectStats stats;

    QSqlQuery volumesQuery(m_db);
    if (!volumesQuery.exec("SELECT COUNT(*) FROM volumes")) {
        qWarning() << "Failed to count volumes" << volumesQuery.lastError();
        return std::nullopt;
    }
    if (volumesQuery.next()) {
        stats.volumeCount = volumesQuery.value(0).toInt();
    }

    QSqlQuery filesQuery(m_db);
    if (!filesQuery.exec("SELECT COUNT(*) FROM files")) {
        qWarning() << "Failed to count files" << filesQuery.lastError();
        return std::nullopt;
    }
    if (filesQuery.next()) {
        stats.fileCount = filesQuery.value(0).toLongLong();
    }

    QSqlQuery bytesQuery(m_db);
    if (!bytesQuery.exec("SELECT COALESCE(SUM(size), 0) FROM files")) {
        qWarning() << "Failed to sum file sizes" << bytesQuery.lastError();
        return std::nullopt;
    }
    if (bytesQuery.next()) {
        stats.totalBytes = bytesQuery.value(0).toLongLong();
    }

    return stats;
}

QList<SearchResult> KatalogueDatabase::search(const QString &queryText,
                                              const SearchFilters &filters,
                                              int limit,
                                              int offset) const {
    QList<SearchResult> results;
    if (!m_db.isOpen()) {
        return results;
    }

    const QString trimmed = queryText.trimmed();
    if (trimmed.isEmpty()) {
        return results;
    }

    QString statement =
        "SELECT files.id, files.directory_id, directories.volume_id, files.name, "
        "directories.full_path || '/' || files.name AS full_path, "
        "volumes.label, files.file_type, files.size, files.mtime "
        "FROM file_fts "
        "JOIN files ON files.id = file_fts.rowid "
        "JOIN directories ON directories.id = files.directory_id "
        "JOIN volumes ON volumes.id = directories.volume_id "
        "WHERE file_fts MATCH ? ";

    if (filters.volumeId.has_value()) {
        statement += "AND directories.volume_id = ? ";
    }

    bool hasFileTypeFilter = false;
    QString fileTypeValue;
    if (filters.fileType.has_value()) {
        fileTypeValue = filters.fileType->trimmed().toLower();
        if (!fileTypeValue.isEmpty()) {
            hasFileTypeFilter = true;
            statement += "AND files.file_type LIKE ? ";
        }
    }

    statement += "ORDER BY files.mtime DESC LIMIT ? OFFSET ?";

    QSqlQuery query(m_db);
    query.prepare(statement);
    const QString matchQuery = trimmed + '*';
    query.addBindValue(matchQuery);

    if (filters.volumeId.has_value()) {
        query.addBindValue(filters.volumeId.value());
    }

    if (hasFileTypeFilter) {
        const QString pattern = fileTypeValue.contains('/')
                                    ? fileTypeValue
                                    : QStringLiteral("%/") + fileTypeValue;
        query.addBindValue(pattern);
    }

    query.addBindValue(limit);
    query.addBindValue(offset);

    if (!query.exec()) {
        qWarning() << "Failed to search" << query.lastError();
        return results;
    }

    while (query.next()) {
        SearchResult result;
        result.fileId = query.value(0).toInt();
        result.directoryId = query.value(1).toInt();
        result.volumeId = query.value(2).toInt();
        result.fileName = query.value(3).toString();
        result.fullPath = query.value(4).toString();
        result.volumeLabel = query.value(5).toString();
        result.fileType = query.value(6).toString();
        result.size = query.value(7).toLongLong();
        result.mtime = QDateTime::fromSecsSinceEpoch(query.value(8).toLongLong(), Qt::UTC);
        results.append(result);
    }

    return results;
}

QList<DirectoryInfo> KatalogueDatabase::listDirectories(int volumeId, int parentId) const {
    QList<DirectoryInfo> directories;
    if (!m_db.isOpen()) {
        return directories;
    }

    QSqlQuery query(m_db);
    if (parentId < 0) {
        query.prepare("SELECT id, volume_id, parent_id, name, full_path "
                      "FROM directories WHERE volume_id = ? AND (parent_id IS NULL OR parent_id = -1) "
                      "ORDER BY name");
        query.addBindValue(volumeId);
    } else {
        query.prepare("SELECT id, volume_id, parent_id, name, full_path "
                      "FROM directories WHERE volume_id = ? AND parent_id = ? "
                      "ORDER BY name");
        query.addBindValue(volumeId);
        query.addBindValue(parentId);
    }

    if (!query.exec()) {
        qWarning() << "Failed to list directories" << query.lastError();
        return directories;
    }

    while (query.next()) {
        DirectoryInfo info;
        info.id = query.value(0).toInt();
        info.volumeId = query.value(1).toInt();
        info.parentId = query.value(2).isNull() ? -1 : query.value(2).toInt();
        info.name = query.value(3).toString();
        info.fullPath = query.value(4).toString();
        directories.append(info);
    }

    return directories;
}

QList<FileInfo> KatalogueDatabase::listFilesInDirectory(int directoryId) const {
    QList<FileInfo> files;
    if (!m_db.isOpen()) {
        return files;
    }

    const QString basePath = directoryFullPath(directoryId);

    QSqlQuery query(m_db);
    query.prepare("SELECT id, directory_id, name, size, mtime, ctime, file_type, hash, attrs "
                  "FROM files WHERE directory_id = ? "
                  "ORDER BY name");
    query.addBindValue(directoryId);

    if (!query.exec()) {
        qWarning() << "Failed to list files" << query.lastError();
        return files;
    }

    while (query.next()) {
        FileInfo info;
        info.id = query.value(0).toInt();
        info.directoryId = query.value(1).toInt();
        info.name = query.value(2).toString();
        info.fullPath = basePath.isEmpty()
                            ? info.name
                            : QDir::cleanPath(basePath + '/' + info.name);
        info.size = query.value(3).toLongLong();
        if (!query.value(4).isNull()) {
            info.mtime = QDateTime::fromSecsSinceEpoch(query.value(4).toLongLong(), Qt::UTC);
        }
        if (!query.value(5).isNull()) {
            info.ctime = QDateTime::fromSecsSinceEpoch(query.value(5).toLongLong(), Qt::UTC);
        }
        info.fileType = query.value(6).toString();
        info.hash = query.value(7).toString();
        info.attrs = static_cast<quint32>(query.value(8).toUInt());
        files.append(info);
    }

    return files;
}

std::optional<DirectoryInfo> KatalogueDatabase::getDirectory(int directoryId) const {
    if (!m_db.isOpen()) {
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT id, volume_id, parent_id, name, full_path FROM directories WHERE id = ?");
    query.addBindValue(directoryId);
    if (!query.exec()) {
        qWarning() << "Failed to get directory" << query.lastError();
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }

    DirectoryInfo info;
    info.id = query.value(0).toInt();
    info.volumeId = query.value(1).toInt();
    info.parentId = query.value(2).isNull() ? -1 : query.value(2).toInt();
    info.name = query.value(3).toString();
    info.fullPath = query.value(4).toString();
    return info;
}

std::optional<QString> KatalogueDatabase::getVolumeLabel(int volumeId) const {
    if (!m_db.isOpen() || volumeId < 0) {
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT label FROM volumes WHERE id = ?");
    query.addBindValue(volumeId);
    if (!query.exec()) {
        qWarning() << "Failed to fetch volume label" << query.lastError();
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }
    return query.value(0).toString();
}

QList<SearchResult> KatalogueDatabase::searchByName(const QString &queryText, int limit, int offset) const {
    SearchFilters filters;
    return search(queryText, filters, limit, offset);
}

std::optional<QString> KatalogueDatabase::getNoteForFile(int fileId) const {
    if (!m_db.isOpen() || fileId < 0) {
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT content FROM notes WHERE target_type = ? AND target_id = ?");
    query.addBindValue(QStringLiteral("file"));
    query.addBindValue(fileId);
    if (!query.exec()) {
        qWarning() << "Failed to fetch file note" << query.lastError();
        return std::nullopt;
    }
    if (query.next()) {
        return query.value(0).toString();
    }
    return std::nullopt;
}

std::optional<QString> KatalogueDatabase::getNoteForDirectory(int directoryId) const {
    if (!m_db.isOpen() || directoryId < 0) {
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT content FROM notes WHERE target_type = ? AND target_id = ?");
    query.addBindValue(QStringLiteral("directory"));
    query.addBindValue(directoryId);
    if (!query.exec()) {
        qWarning() << "Failed to fetch directory note" << query.lastError();
        return std::nullopt;
    }
    if (query.next()) {
        return query.value(0).toString();
    }
    return std::nullopt;
}

bool KatalogueDatabase::setNoteForFile(int fileId, const QString &content) {
    if (!m_db.isOpen() || fileId < 0) {
        return false;
    }
    const QString trimmed = content.trimmed();
    QSqlQuery query(m_db);
    if (trimmed.isEmpty()) {
        query.prepare("DELETE FROM notes WHERE target_type = ? AND target_id = ?");
        query.addBindValue(QStringLiteral("file"));
        query.addBindValue(fileId);
        if (!query.exec()) {
            qWarning() << "Failed to delete file note" << query.lastError();
            return false;
        }
        return true;
    }

    query.prepare("SELECT id FROM notes WHERE target_type = ? AND target_id = ?");
    query.addBindValue(QStringLiteral("file"));
    query.addBindValue(fileId);
    if (!query.exec()) {
        qWarning() << "Failed to query file note" << query.lastError();
        return false;
    }
    if (query.next()) {
        const int noteId = query.value(0).toInt();
        QSqlQuery update(m_db);
        update.prepare("UPDATE notes SET content = ? WHERE id = ?");
        update.addBindValue(trimmed);
        update.addBindValue(noteId);
        if (!update.exec()) {
            qWarning() << "Failed to update file note" << update.lastError();
            return false;
        }
        return true;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO notes (target_type, target_id, content) VALUES (?, ?, ?)");
    insert.addBindValue(QStringLiteral("file"));
    insert.addBindValue(fileId);
    insert.addBindValue(trimmed);
    if (!insert.exec()) {
        qWarning() << "Failed to insert file note" << insert.lastError();
        return false;
    }
    return true;
}

bool KatalogueDatabase::setNoteForDirectory(int directoryId, const QString &content) {
    if (!m_db.isOpen() || directoryId < 0) {
        return false;
    }
    const QString trimmed = content.trimmed();
    QSqlQuery query(m_db);
    if (trimmed.isEmpty()) {
        query.prepare("DELETE FROM notes WHERE target_type = ? AND target_id = ?");
        query.addBindValue(QStringLiteral("directory"));
        query.addBindValue(directoryId);
        if (!query.exec()) {
            qWarning() << "Failed to delete directory note" << query.lastError();
            return false;
        }
        return true;
    }

    query.prepare("SELECT id FROM notes WHERE target_type = ? AND target_id = ?");
    query.addBindValue(QStringLiteral("directory"));
    query.addBindValue(directoryId);
    if (!query.exec()) {
        qWarning() << "Failed to query directory note" << query.lastError();
        return false;
    }
    if (query.next()) {
        const int noteId = query.value(0).toInt();
        QSqlQuery update(m_db);
        update.prepare("UPDATE notes SET content = ? WHERE id = ?");
        update.addBindValue(trimmed);
        update.addBindValue(noteId);
        if (!update.exec()) {
            qWarning() << "Failed to update directory note" << update.lastError();
            return false;
        }
        return true;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO notes (target_type, target_id, content) VALUES (?, ?, ?)");
    insert.addBindValue(QStringLiteral("directory"));
    insert.addBindValue(directoryId);
    insert.addBindValue(trimmed);
    if (!insert.exec()) {
        qWarning() << "Failed to insert directory note" << insert.lastError();
        return false;
    }
    return true;
}

bool KatalogueDatabase::addTagToFile(int fileId, const QString &key, const QString &value) {
    if (!m_db.isOpen() || fileId < 0) {
        return false;
    }
    const QString trimmedKey = key.trimmed();
    if (trimmedKey.isEmpty()) {
        return false;
    }
    const QString trimmedValue = value.trimmed();

    int tagId = -1;
    QSqlQuery findTag(m_db);
    findTag.prepare("SELECT id FROM tags WHERE key = ? AND value = ?");
    findTag.addBindValue(trimmedKey);
    findTag.addBindValue(trimmedValue);
    if (!findTag.exec()) {
        qWarning() << "Failed to lookup tag" << findTag.lastError();
        return false;
    }
    if (findTag.next()) {
        tagId = findTag.value(0).toInt();
    } else {
        QSqlQuery insertTag(m_db);
        insertTag.prepare("INSERT INTO tags (key, value) VALUES (?, ?)");
        insertTag.addBindValue(trimmedKey);
        insertTag.addBindValue(trimmedValue);
        if (!insertTag.exec()) {
            qWarning() << "Failed to insert tag" << insertTag.lastError();
            return false;
        }
        tagId = insertTag.lastInsertId().toInt();
    }

    QSqlQuery link(m_db);
    link.prepare("INSERT OR IGNORE INTO file_tags (file_id, tag_id) VALUES (?, ?)");
    link.addBindValue(fileId);
    link.addBindValue(tagId);
    if (!link.exec()) {
        qWarning() << "Failed to link tag to file" << link.lastError();
        return false;
    }
    return true;
}

bool KatalogueDatabase::removeTagFromFile(int fileId, const QString &key, const QString &value) {
    if (!m_db.isOpen() || fileId < 0) {
        return false;
    }
    const QString trimmedKey = key.trimmed();
    if (trimmedKey.isEmpty()) {
        return false;
    }
    const QString trimmedValue = value.trimmed();

    QSqlQuery findTag(m_db);
    findTag.prepare("SELECT id FROM tags WHERE key = ? AND value = ?");
    findTag.addBindValue(trimmedKey);
    findTag.addBindValue(trimmedValue);
    if (!findTag.exec()) {
        qWarning() << "Failed to lookup tag for removal" << findTag.lastError();
        return false;
    }
    if (!findTag.next()) {
        return true;
    }
    const int tagId = findTag.value(0).toInt();

    QSqlQuery unlink(m_db);
    unlink.prepare("DELETE FROM file_tags WHERE file_id = ? AND tag_id = ?");
    unlink.addBindValue(fileId);
    unlink.addBindValue(tagId);
    if (!unlink.exec()) {
        qWarning() << "Failed to unlink tag from file" << unlink.lastError();
        return false;
    }
    return true;
}

QList<QPair<QString, QString>> KatalogueDatabase::tagsForFile(int fileId) const {
    QList<QPair<QString, QString>> tags;
    if (!m_db.isOpen() || fileId < 0) {
        return tags;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT tags.key, tags.value "
                  "FROM tags "
                  "JOIN file_tags ON file_tags.tag_id = tags.id "
                  "WHERE file_tags.file_id = ? "
                  "ORDER BY tags.key, tags.value");
    query.addBindValue(fileId);
    if (!query.exec()) {
        qWarning() << "Failed to list tags for file" << query.lastError();
        return tags;
    }
    while (query.next()) {
        tags.append(qMakePair(query.value(0).toString(), query.value(1).toString()));
    }
    return tags;
}

QList<SearchResult> KatalogueDatabase::listAllFiles(const std::optional<int> &volumeId) const {
    QList<SearchResult> results;
    if (!m_db.isOpen()) {
        return results;
    }

    QSqlQuery query(m_db);
    if (volumeId.has_value()) {
        query.prepare(
            "SELECT files.id, files.directory_id, directories.volume_id, files.name, "
            "directories.full_path || '/' || files.name AS full_path, "
            "volumes.label, files.file_type, files.size, files.mtime "
            "FROM files "
            "JOIN directories ON directories.id = files.directory_id "
            "JOIN volumes ON volumes.id = directories.volume_id "
            "WHERE directories.volume_id = ? "
            "ORDER BY volumes.label, directories.full_path, files.name");
        query.addBindValue(volumeId.value());
    } else {
        query.prepare(
            "SELECT files.id, files.directory_id, directories.volume_id, files.name, "
            "directories.full_path || '/' || files.name AS full_path, "
            "volumes.label, files.file_type, files.size, files.mtime "
            "FROM files "
            "JOIN directories ON directories.id = files.directory_id "
            "JOIN volumes ON volumes.id = directories.volume_id "
            "ORDER BY volumes.label, directories.full_path, files.name");
    }

    if (!query.exec()) {
        qWarning() << "Failed to list all files" << query.lastError();
        return results;
    }

    while (query.next()) {
        SearchResult result;
        result.fileId = query.value(0).toInt();
        result.directoryId = query.value(1).toInt();
        result.volumeId = query.value(2).toInt();
        result.fileName = query.value(3).toString();
        result.fullPath = query.value(4).toString();
        result.volumeLabel = query.value(5).toString();
        result.fileType = query.value(6).toString();
        result.size = query.value(7).toLongLong();
        result.mtime = QDateTime::fromSecsSinceEpoch(query.value(8).toLongLong(), Qt::UTC);
        results.append(result);
    }

    return results;
}

int KatalogueDatabase::createVirtualFolder(const QString &name, int parentId) {
    if (!m_db.isOpen()) {
        return -1;
    }
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        return -1;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO virtual_folders (parent_id, name) VALUES (?, ?)");
    if (parentId < 0) {
        query.addBindValue(QVariant(QVariant::Int));
    } else {
        query.addBindValue(parentId);
    }
    query.addBindValue(trimmed);
    if (!query.exec()) {
        qWarning() << "Failed to create virtual folder" << query.lastError();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool KatalogueDatabase::renameVirtualFolder(int folderId, const QString &newName) {
    if (!m_db.isOpen() || folderId < 0) {
        return false;
    }
    const QString trimmed = newName.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }
    QSqlQuery query(m_db);
    query.prepare("UPDATE virtual_folders SET name = ? WHERE id = ?");
    query.addBindValue(trimmed);
    query.addBindValue(folderId);
    if (!query.exec()) {
        qWarning() << "Failed to rename virtual folder" << query.lastError();
        return false;
    }
    return true;
}

bool KatalogueDatabase::deleteVirtualFolder(int folderId) {
    if (!m_db.isOpen() || folderId < 0) {
        return false;
    }
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM virtual_folders WHERE id = ?");
    query.addBindValue(folderId);
    if (!query.exec()) {
        qWarning() << "Failed to delete virtual folder" << query.lastError();
        return false;
    }
    return true;
}

QList<VirtualFolderInfo> KatalogueDatabase::listVirtualFolders(int parentId) const {
    QList<VirtualFolderInfo> folders;
    if (!m_db.isOpen()) {
        return folders;
    }

    QSqlQuery query(m_db);
    if (parentId < 0) {
        query.prepare("SELECT id, parent_id, name FROM virtual_folders "
                      "WHERE parent_id IS NULL OR parent_id = -1 "
                      "ORDER BY name COLLATE NOCASE");
    } else {
        query.prepare("SELECT id, parent_id, name FROM virtual_folders "
                      "WHERE parent_id = ? ORDER BY name COLLATE NOCASE");
        query.addBindValue(parentId);
    }

    if (!query.exec()) {
        qWarning() << "Failed to list virtual folders" << query.lastError();
        return folders;
    }
    while (query.next()) {
        VirtualFolderInfo info;
        info.id = query.value(0).toInt();
        info.parentId = query.value(1).isNull() ? -1 : query.value(1).toInt();
        info.name = query.value(2).toString();
        folders.append(info);
    }
    return folders;
}

std::optional<VirtualFolderInfo> KatalogueDatabase::getVirtualFolder(int folderId) const {
    if (!m_db.isOpen() || folderId < 0) {
        return std::nullopt;
    }
    QSqlQuery query(m_db);
    query.prepare("SELECT id, parent_id, name FROM virtual_folders WHERE id = ?");
    query.addBindValue(folderId);
    if (!query.exec()) {
        qWarning() << "Failed to get virtual folder" << query.lastError();
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }
    VirtualFolderInfo info;
    info.id = query.value(0).toInt();
    info.parentId = query.value(1).isNull() ? -1 : query.value(1).toInt();
    info.name = query.value(2).toString();
    return info;
}

bool KatalogueDatabase::addFileToVirtualFolder(int folderId, int fileId) {
    if (!m_db.isOpen() || folderId < 0 || fileId < 0) {
        return false;
    }
    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO virtual_folder_items (folder_id, file_id) VALUES (?, ?)");
    query.addBindValue(folderId);
    query.addBindValue(fileId);
    if (!query.exec()) {
        qWarning() << "Failed to add file to virtual folder" << query.lastError();
        return false;
    }
    return true;
}

bool KatalogueDatabase::removeFileFromVirtualFolder(int folderId, int fileId) {
    if (!m_db.isOpen() || folderId < 0 || fileId < 0) {
        return false;
    }
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM virtual_folder_items WHERE folder_id = ? AND file_id = ?");
    query.addBindValue(folderId);
    query.addBindValue(fileId);
    if (!query.exec()) {
        qWarning() << "Failed to remove file from virtual folder" << query.lastError();
        return false;
    }
    return true;
}

QList<SearchResult> KatalogueDatabase::listVirtualFolderItems(int folderId) const {
    QList<SearchResult> results;
    if (!m_db.isOpen() || folderId < 0) {
        return results;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT files.id, files.directory_id, directories.volume_id, files.name, "
        "directories.full_path || '/' || files.name AS full_path, "
        "volumes.label, files.file_type, files.size, files.mtime "
        "FROM virtual_folder_items "
        "JOIN files ON files.id = virtual_folder_items.file_id "
        "JOIN directories ON directories.id = files.directory_id "
        "JOIN volumes ON volumes.id = directories.volume_id "
        "WHERE virtual_folder_items.folder_id = ? "
        "ORDER BY volumes.label, directories.full_path, files.name");
    query.addBindValue(folderId);
    if (!query.exec()) {
        qWarning() << "Failed to list virtual folder items" << query.lastError();
        return results;
    }

    while (query.next()) {
        SearchResult result;
        result.fileId = query.value(0).toInt();
        result.directoryId = query.value(1).toInt();
        result.volumeId = query.value(2).toInt();
        result.fileName = query.value(3).toString();
        result.fullPath = query.value(4).toString();
        result.volumeLabel = query.value(5).toString();
        result.fileType = query.value(6).toString();
        result.size = query.value(7).toLongLong();
        result.mtime = QDateTime::fromSecsSinceEpoch(query.value(8).toLongLong(), Qt::UTC);
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
