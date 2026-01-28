#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>

#include "katalogue_types.h"

class KatalogueDatabase;

class KatalogueScanner : public QObject {
    Q_OBJECT
public:
    struct Options {
        bool computeHashes = false;
        QStringList excludePatterns;
    };

    explicit KatalogueScanner(KatalogueDatabase *db, QObject *parent = nullptr);

    void setOptions(const Options &options);

public slots:
    void scanVolume(const QString &mountPoint, VolumeInfo volumeInfo);

signals:
    void progress(int percent, quint64 filesProcessed);
    void finished(bool success, const QString &errorString = QString());

private:
    bool shouldSkip(const QString &relativePath) const;
    QString detectFileType(const QString &fileName) const;
    QString computeHash(const QString &filePath) const;

    KatalogueDatabase *m_db = nullptr;
    Options m_options;
    QHash<QString, int> m_directoryCache;
};
