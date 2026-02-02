#pragma once

#include <memory>

#include <QObject>
#include <QHash>
#include <QVariantList>
#include <QVariantMap>

#include "katalogue_settings.h"

class QDBusInterface;

class KatalogueClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList volumes READ volumes NOTIFY volumesChanged)
    Q_PROPERTY(QVariantList searchResults READ searchResults NOTIFY searchResultsChanged)
    Q_PROPERTY(int selectedVolumeId READ selectedVolumeId WRITE setSelectedVolumeId NOTIFY selectedVolumeIdChanged)
    Q_PROPERTY(int selectedDirectoryId READ selectedDirectoryId WRITE setSelectedDirectoryId NOTIFY selectedDirectoryIdChanged)
    Q_PROPERTY(int selectedFileId READ selectedFileId WRITE setSelectedFileId NOTIFY selectedFileIdChanged)
    Q_PROPERTY(QVariantList directoryEntries READ directoryEntries NOTIFY directoryEntriesChanged)
    Q_PROPERTY(QVariantList fileEntries READ fileEntries NOTIFY fileEntriesChanged)
    Q_PROPERTY(QVariantList activeScans READ activeScans NOTIFY activeScansChanged)
    Q_PROPERTY(QString projectPath READ projectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QVariantMap projectInfo READ projectInfo NOTIFY projectInfoChanged)
    Q_PROPERTY(QVariantList virtualFolders READ virtualFolders NOTIFY virtualFoldersChanged)
    Q_PROPERTY(int selectedVirtualFolderId READ selectedVirtualFolderId WRITE setSelectedVirtualFolderId NOTIFY selectedVirtualFolderIdChanged)
    Q_PROPERTY(QVariantList virtualFolderItems READ virtualFolderItems NOTIFY virtualFolderItemsChanged)
    Q_PROPERTY(bool scannerIncludeHidden READ scannerIncludeHidden WRITE setScannerIncludeHidden NOTIFY settingsChanged)
    Q_PROPERTY(bool scannerFollowSymlinks READ scannerFollowSymlinks WRITE setScannerFollowSymlinks NOTIFY settingsChanged)
    Q_PROPERTY(bool scannerComputeHashes READ scannerComputeHashes WRITE setScannerComputeHashes NOTIFY settingsChanged)
    Q_PROPERTY(int scannerMaxDepth READ scannerMaxDepth WRITE setScannerMaxDepth NOTIFY settingsChanged)
    Q_PROPERTY(QString scannerExcludePatternsString READ scannerExcludePatternsString WRITE setScannerExcludePatternsString NOTIFY settingsChanged)
    Q_PROPERTY(bool uiConfirmVirtualFolderDelete READ uiConfirmVirtualFolderDelete WRITE setUiConfirmVirtualFolderDelete NOTIFY settingsChanged)

public:
    explicit KatalogueClient(QObject *parent = nullptr);

    QVariantList volumes() const;
    QVariantList searchResults() const;
    int selectedVolumeId() const;
    void setSelectedVolumeId(int volumeId);
    int selectedDirectoryId() const;
    void setSelectedDirectoryId(int directoryId);
    int selectedFileId() const;
    void setSelectedFileId(int fileId);
    QVariantList directoryEntries() const;
    QVariantList fileEntries() const;
    QVariantList activeScans() const;
    QString projectPath() const;
    QVariantMap projectInfo() const;
    QVariantList virtualFolders() const;
    int selectedVirtualFolderId() const;
    void setSelectedVirtualFolderId(int folderId);
    QVariantList virtualFolderItems() const;
    bool scannerIncludeHidden() const;
    void setScannerIncludeHidden(bool value);
    bool scannerFollowSymlinks() const;
    void setScannerFollowSymlinks(bool value);
    bool scannerComputeHashes() const;
    void setScannerComputeHashes(bool value);
    int scannerMaxDepth() const;
    void setScannerMaxDepth(int depth);
    QString scannerExcludePatternsString() const;
    void setScannerExcludePatternsString(const QString &patterns);
    bool uiConfirmVirtualFolderDelete() const;
    void setUiConfirmVirtualFolderDelete(bool value);

    Q_INVOKABLE void openProject(const QString &path);
    Q_INVOKABLE int startScan(const QString &rootPath);
    Q_INVOKABLE void cancelScan(int scanId);
    Q_INVOKABLE QVariantMap scanStatus(int scanId) const;
    Q_INVOKABLE void refreshVolumes();
    Q_INVOKABLE void searchByName(const QString &query, int limit = 100, int offset = 0);
    Q_INVOKABLE void search(const QString &query,
                            int volumeId = -1,
                            const QString &fileType = QString(),
                            int limit = 200,
                            int offset = 0);
    Q_INVOKABLE void jumpToResult(int volumeId, int directoryId);
    Q_INVOKABLE QString getFileNote(int fileId);
    Q_INVOKABLE void setFileNote(int fileId, const QString &content);
    Q_INVOKABLE QVariantList getFileTags(int fileId);
    Q_INVOKABLE void addFileTag(int fileId, const QString &key, const QString &value);
    Q_INVOKABLE void removeFileTag(int fileId, const QString &key, const QString &value);
    Q_INVOKABLE void loadVirtualFolderChildren(int parentId);
    Q_INVOKABLE int createVirtualFolder(const QString &name, int parentId);
    Q_INVOKABLE void renameVirtualFolder(int folderId, const QString &newName);
    Q_INVOKABLE void deleteVirtualFolder(int folderId);
    Q_INVOKABLE void loadVirtualFolderItems(int folderId);
    Q_INVOKABLE void addFileToVirtualFolder(int folderId, int fileId);
    Q_INVOKABLE void removeFileFromVirtualFolder(int folderId, int fileId);
    Q_INVOKABLE void jumpToVirtualFolderItem(int volumeId, int directoryId);
    Q_INVOKABLE void renameVolume(int volumeId, const QString &newLabel);
    Q_INVOKABLE void copyTextToClipboard(const QString &text);
    Q_INVOKABLE QVariantMap getFileInfo(int fileId) const;
    Q_INVOKABLE void refreshProjectInfo();
    Q_INVOKABLE void loadRootForVolume(int volumeId);
    Q_INVOKABLE void loadDirectory(int directoryId);

signals:
    void volumesChanged();
    void searchResultsChanged();
    void selectedVolumeIdChanged();
    void selectedDirectoryIdChanged();
    void selectedFileIdChanged();
    void settingsChanged();
    void directoryEntriesChanged();
    void fileEntriesChanged();
    void activeScansChanged();
    void projectPathChanged();
    void projectInfoChanged();
    void virtualFoldersChanged();
    void selectedVirtualFolderIdChanged();
    void virtualFolderItemsChanged();
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
    QVariantList m_virtualFolders;
    int m_selectedVirtualFolderId = -1;
    QVariantList m_virtualFolderItems;
    KatalogueSettings m_settings;
    QHash<int, ScanInfo> m_scans;
    QVariantList m_activeScans;
    int m_selectedVolumeId = -1;
    int m_selectedDirectoryId = -1;
    int m_selectedFileId = -1;
    bool m_signalsConnected = false;
};
