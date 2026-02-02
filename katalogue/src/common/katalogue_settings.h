#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class KatalogueSettings : public QObject {
    Q_OBJECT
public:
    explicit KatalogueSettings(QObject *parent = nullptr);

    bool scannerIncludeHidden() const;
    void setScannerIncludeHidden(bool value);

    bool scannerFollowSymlinks() const;
    void setScannerFollowSymlinks(bool value);

    bool scannerComputeHashes() const;
    void setScannerComputeHashes(bool value);

    int scannerMaxDepth() const;
    void setScannerMaxDepth(int depth);

    QStringList scannerExcludePatterns() const;
    void setScannerExcludePatterns(const QStringList &patterns);

    bool uiConfirmVirtualFolderDelete() const;
    void setUiConfirmVirtualFolderDelete(bool value);

signals:
    void scannerSettingsChanged();
    void uiSettingsChanged();
};
