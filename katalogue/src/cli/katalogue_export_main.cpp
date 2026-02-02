#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <memory>

#include "katalogue_database.h"
#include "katalogue_version.h"

namespace {
struct Options {
    bool listVolumes = false;
    bool listFiles = false;
    bool hasVolumeId = false;
    int volumeId = -1;
    QString search;
    QString format = QStringLiteral("csv");
    QString outputPath;
    QString catalogPath;
};

void printUsage(QTextStream &out) {
    out << "Usage: katalogue-export [OPTIONS] <catalog-file>\n"
           "Options:\n"
           "  --list-volumes                 Export volumes info\n"
           "  --list-files                   Export all files\n"
           "  --volume-id <id>               Restrict files export to a volume\n"
           "  --search <query>               Restrict files export to search results\n"
           "  --format <csv|json>            Output format (default: csv)\n"
           "  --output <path>                Output file path (default: stdout)\n";
}

QString csvEscape(const QString &value) {
    QString escaped = value;
    escaped.replace('"', "\"\"");
    return QStringLiteral("\"") + escaped + QStringLiteral("\"");
}

void writeCsvRow(QTextStream &out, const QStringList &values) {
    QStringList escaped;
    escaped.reserve(values.size());
    for (const auto &value : values) {
        escaped.append(csvEscape(value));
    }
    out << escaped.join(',') << '\n';
}
} // namespace

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QTextStream err(stderr);

    Options options;
    const QStringList args = app.arguments();
    for (int i = 1; i < args.size(); ++i) {
        const QString arg = args.at(i);
        if (arg == QStringLiteral("--help") || arg == QStringLiteral("-h")) {
            printUsage(err);
            return 0;
        }
        if (arg == QStringLiteral("--version")) {
            QTextStream out(stdout);
            out << "Katalogue export tool " << KATALOGUE_VERSION_STRING << Qt::endl;
            return 0;
        }
        if (arg == QStringLiteral("--list-volumes")) {
            options.listVolumes = true;
            continue;
        }
        if (arg == QStringLiteral("--list-files")) {
            options.listFiles = true;
            continue;
        }
        if (arg == QStringLiteral("--volume-id") && i + 1 < args.size()) {
            options.hasVolumeId = true;
            options.volumeId = args.at(++i).toInt();
            continue;
        }
        if (arg == QStringLiteral("--search") && i + 1 < args.size()) {
            options.search = args.at(++i);
            continue;
        }
        if (arg == QStringLiteral("--format") && i + 1 < args.size()) {
            options.format = args.at(++i).toLower();
            continue;
        }
        if (arg == QStringLiteral("--output") && i + 1 < args.size()) {
            options.outputPath = args.at(++i);
            continue;
        }
        if (arg.startsWith(QStringLiteral("--"))) {
            err << "Unknown option: " << arg << '\n';
            printUsage(err);
            return 1;
        }
        if (options.catalogPath.isEmpty()) {
            options.catalogPath = arg;
        } else {
            err << "Unexpected argument: " << arg << '\n';
            printUsage(err);
            return 1;
        }
    }

    if (!options.listVolumes && !options.listFiles) {
        err << "Specify --list-volumes or --list-files.\n";
        printUsage(err);
        return 1;
    }

    if (options.catalogPath.isEmpty()) {
        err << "Missing catalog file path.\n";
        printUsage(err);
        return 1;
    }

    if (options.format != QStringLiteral("csv") && options.format != QStringLiteral("json")) {
        err << "Unsupported format: " << options.format << '\n';
        return 1;
    }

    KatalogueDatabase db;
    if (!db.openProject(options.catalogPath)) {
        err << "Failed to open catalog: " << options.catalogPath << '\n';
        return 1;
    }

    std::unique_ptr<QFile> outputFile;
    QTextStream out(stdout);
    if (!options.outputPath.isEmpty()) {
        outputFile = std::make_unique<QFile>(options.outputPath);
        if (!outputFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            err << "Failed to open output file: " << options.outputPath << '\n';
            return 1;
        }
        out.setDevice(outputFile.get());
    }

    if (options.listVolumes) {
        const auto volumes = db.listVolumes();
        if (options.format == QStringLiteral("json")) {
            QJsonArray array;
            for (const auto &volume : volumes) {
                QJsonObject obj;
                obj.insert(QStringLiteral("id"), volume.id);
                obj.insert(QStringLiteral("label"), volume.label);
                obj.insert(QStringLiteral("description"), volume.description);
                obj.insert(QStringLiteral("fsUuid"), volume.fsUuid);
                obj.insert(QStringLiteral("fsType"), volume.fsType);
                obj.insert(QStringLiteral("totalSize"), static_cast<double>(volume.totalSize));
                obj.insert(QStringLiteral("createdAt"), volume.createdAt.isValid()
                                                       ? volume.createdAt.toString(Qt::ISODate)
                                                       : QString());
                obj.insert(QStringLiteral("updatedAt"), volume.updatedAt.isValid()
                                                       ? volume.updatedAt.toString(Qt::ISODate)
                                                       : QString());
                array.append(obj);
            }
            out << QJsonDocument(array).toJson(QJsonDocument::Compact) << '\n';
        } else {
            writeCsvRow(out, {"id", "label", "description", "fs_uuid", "fs_type", "total_size", "created_at", "updated_at"});
            for (const auto &volume : volumes) {
                writeCsvRow(out,
                            {
                                QString::number(volume.id),
                                volume.label,
                                volume.description,
                                volume.fsUuid,
                                volume.fsType,
                                QString::number(volume.totalSize),
                                volume.createdAt.isValid() ? volume.createdAt.toString(Qt::ISODate) : QString(),
                                volume.updatedAt.isValid() ? volume.updatedAt.toString(Qt::ISODate) : QString()
                            });
            }
        }
        return 0;
    }

    QList<SearchResult> files;
    if (!options.search.trimmed().isEmpty()) {
        KatalogueDatabase::SearchFilters filters;
        if (options.hasVolumeId) {
            filters.volumeId = options.volumeId;
        }
        files = db.search(options.search, filters, 100000, 0);
    } else {
        std::optional<int> volumeFilter;
        if (options.hasVolumeId) {
            volumeFilter = options.volumeId;
        }
        files = db.listAllFiles(volumeFilter);
    }

    if (options.format == QStringLiteral("json")) {
        QJsonArray array;
        for (const auto &file : files) {
            QJsonObject obj;
            obj.insert(QStringLiteral("fileId"), file.fileId);
            obj.insert(QStringLiteral("volumeId"), file.volumeId);
            obj.insert(QStringLiteral("volumeLabel"), file.volumeLabel);
            obj.insert(QStringLiteral("directoryId"), file.directoryId);
            obj.insert(QStringLiteral("fullPath"), file.fullPath);
            obj.insert(QStringLiteral("fileName"), file.fileName);
            obj.insert(QStringLiteral("fileType"), file.fileType);
            obj.insert(QStringLiteral("size"), static_cast<double>(file.size));
            obj.insert(QStringLiteral("mtime"), file.mtime.isValid()
                                              ? file.mtime.toString(Qt::ISODate)
                                              : QString());
            array.append(obj);
        }
        out << QJsonDocument(array).toJson(QJsonDocument::Compact) << '\n';
    } else {
        writeCsvRow(out,
                    {"file_id", "volume_id", "volume_label", "directory_id", "full_path", "file_name", "file_type", "size", "mtime"});
        for (const auto &file : files) {
            writeCsvRow(out,
                        {
                            QString::number(file.fileId),
                            QString::number(file.volumeId),
                            file.volumeLabel,
                            QString::number(file.directoryId),
                            file.fullPath,
                            file.fileName,
                            file.fileType,
                            QString::number(file.size),
                            file.mtime.isValid() ? file.mtime.toString(Qt::ISODate) : QString()
                        });
        }
    }

    return 0;
}
