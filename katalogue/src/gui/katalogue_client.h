#pragma once

#include <memory>

#include <QObject>
#include <QHash>
#include <QVariantList>
#include <QVariantMap>

class QDBusInterface;

class KatalogueClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList volumes READ volumes NOTIFY volumesChanged)
    Q_PROPERTY(QVariantList searchResults READ searchResults NOTIFY searchResultsChanged)
    Q_PROPERTY(int selectedVolumeId READ selectedVolumeId WRITE setSelectedVolumeId NOTIFY selectedVolumeIdChanged)
    Q_PROPERTY(int selectedDirectoryId READ selectedDirectoryId WRITE setSelectedDirectoryId NOTIFY selectedDirectoryIdChanged)
    Q_PROPERTY(QVariantList directoryEntries READ directoryEntries NOTIFY directoryEntriesChanged)
    Q_PROPERTY(QVariantList fileEntries READ fileEntries NOTIFY fileEntriesChanged)
    Q_PROPERTY(QVariantList activeScans READ activeScans NOTIFY activeScansChanged)
    Q_PROPERTY(QString projectPath READ projectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QVariantMap projectInfo READ projectInfo NOTIFY projectInfoChanged)

public:
    explicit KatalogueClient(QObject *parent = nullptr);

    QVariantList volumes() const;
    QVariantList searchResults() const;
    int selectedVolumeId() const;
    void setSelectedVolumeId(int volumeId);
    int selectedDirectoryId() const;
    void setSelectedDirectoryId(int directoryId);
    QVariantList directoryEntries() const;
    QVariantList fileEntries() const;
    QVariantList activeScans() const;
    QString projectPath() const;
    QVariantMap projectInfo() const;

    Q_INVOKABLE void openProject(const QString &path);
    Q_INVOKABLE int startScan(const QString &rootPath);
    Q_INVOKABLE void cancelScan(int scanId);
    Q_INVOKABLE QVariantMap scanStatus(int scanId) const;
    Q_INVOKABLE void refreshVolumes();
    Q_INVOKABLE void searchByName(const QString &query, int limit = 100, int offset = 0);
    Q_INVOKABLE void refreshProjectInfo();
    Q_INVOKABLE void loadRootForVolume(int volumeId);
    Q_INVOKABLE void loadDirectory(int directoryId);

signals:
    void volumesChanged();
    void searchResultsChanged();
    void selectedVolumeIdChanged();
    void selectedDirectoryIdChanged();
    void directoryEntriesChanged();
    void fileEntriesChanged();
    void activeScansChanged();
    void projectPathChanged();
    void projectInfoChanged();
    void scanProgress(uint scanId, const QString &path, int directories, int files, qint64 bytes);
    void scanFinished(uint scanId, const QString &status);

private:
    struct ScanInfo {
        int id = 0;
        QString rootPath;
        QString status;
        int percent = -1;
        quint64 directories = 0;
        quint64 files = 0;
        quint64 bytes = 0;
        QString errorString;
    };

    bool ensureInterface();
    void connectSignals();
    void rebuildActiveScans();

private slots:
    void onScanProgress(uint scanId, const QString &path, int directories, int files, qint64 bytes);
    void onScanFinished(uint scanId, const QString &status);

    std::unique_ptr<QDBusInterface> m_iface;
    QVariantList m_volumes;
    QVariantList m_searchResults;
    QVariantList m_directoryEntries;
    QVariantList m_fileEntries;
    QString m_projectPath;
    QVariantMap m_projectInfo;
    QHash<int, ScanInfo> m_scans;
    QVariantList m_activeScans;
    int m_selectedVolumeId = -1;
    int m_selectedDirectoryId = -1;
    bool m_signalsConnected = false;
};
