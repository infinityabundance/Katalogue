#pragma once

#include <atomic>
#include <functional>

#include <QString>
#include <QStringList>

#include "katalogue_database.h"

struct ScanOptions {
    int maxDepth = -1;
    bool followSymlinks = false;
    bool includeHidden = false;
    QStringList excludePatterns;
};

struct ScanStats {
    int directories = 0;
    int files = 0;
    qint64 totalBytes = 0;
};

class KatalogueScanner {
public:
    KatalogueScanner();

    using ProgressCallback = std::function<bool(const QString &currentPath,
                                                 const ScanStats &stats)>;

    bool scan(const QString &rootPath,
              KatalogueDatabase &db,
              VolumeInfo volumeInfo,
              const ScanOptions &options = {},
              ProgressCallback progress = {});

    void cancel();

private:
    bool isExcluded(const QString &relativePath,
                    const QString &name,
                    const ScanOptions &options) const;
    bool withinDepth(const QString &relativePath, int maxDepth) const;

    std::atomic_bool m_cancelled{false};
};
