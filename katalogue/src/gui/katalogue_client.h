#pragma once

#include <memory>

#include <QObject>
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

    Q_INVOKABLE bool openProject(const QString &path);
    Q_INVOKABLE uint startScan(const QString &rootPath);
    Q_INVOKABLE bool cancelScan(uint scanId);
    Q_INVOKABLE QVariantMap scanStatus(uint scanId) const;
    Q_INVOKABLE void refreshVolumes();
    Q_INVOKABLE void searchByName(const QString &query, int limit = 100, int offset = 0);
    Q_INVOKABLE void loadRootForVolume(int volumeId);
    Q_INVOKABLE void loadDirectory(int directoryId);

signals:
    void volumesChanged();
    void searchResultsChanged();
    void selectedVolumeIdChanged();
    void selectedDirectoryIdChanged();
    void directoryEntriesChanged();
    void fileEntriesChanged();
    void scanProgress(uint scanId, const QString &path, int directories, int files, qint64 bytes);
    void scanFinished(uint scanId, const QString &status);

private:
    bool ensureInterface();
    void connectSignals();

    std::unique_ptr<QDBusInterface> m_iface;
    QVariantList m_volumes;
    QVariantList m_searchResults;
    QVariantList m_directoryEntries;
    QVariantList m_fileEntries;
    int m_selectedVolumeId = -1;
    int m_selectedDirectoryId = -1;
    bool m_signalsConnected = false;
};
