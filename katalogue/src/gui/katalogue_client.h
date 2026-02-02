#pragma once

#include <QObject>
#include <QVariantList>

class QDBusInterface;

class KatalogueClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList volumes READ volumes NOTIFY volumesChanged)
    Q_PROPERTY(QVariantList searchResults READ searchResults NOTIFY searchResultsChanged)

public:
    explicit KatalogueClient(QObject *parent = nullptr);

    QVariantList volumes() const;
    QVariantList searchResults() const;

    Q_INVOKABLE bool openProject(const QString &path);
    Q_INVOKABLE uint startScan(const QString &rootPath);
    Q_INVOKABLE bool cancelScan(uint scanId);
    Q_INVOKABLE QVariantMap scanStatus(uint scanId) const;
    Q_INVOKABLE void refreshVolumes();
    Q_INVOKABLE void searchByName(const QString &query, int limit = 100, int offset = 0);

signals:
    void volumesChanged();
    void searchResultsChanged();
    void scanProgress(uint scanId, const QString &path, int directories, int files, qint64 bytes);
    void scanFinished(uint scanId, const QString &status);

private:
    bool ensureInterface();
    void connectSignals();

    std::unique_ptr<QDBusInterface> m_iface;
    QVariantList m_volumes;
    QVariantList m_searchResults;
    bool m_signalsConnected = false;
};
