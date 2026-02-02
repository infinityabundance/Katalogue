#include "katalogue_settings.h"

#include <QSettings>

namespace {
QSettings &settings() {
    static QSettings s(QSettings::IniFormat,
                       QSettings::UserScope,
                       QStringLiteral("Katalogue"),
                       QStringLiteral("Katalogue"));
    return s;
}
} // namespace

KatalogueSettings::KatalogueSettings(QObject *parent)
    : QObject(parent) {}

bool KatalogueSettings::scannerIncludeHidden() const {
    return settings().value(QStringLiteral("scanner/includeHidden"), false).toBool();
}

void KatalogueSettings::setScannerIncludeHidden(bool value) {
    settings().setValue(QStringLiteral("scanner/includeHidden"), value);
    emit scannerSettingsChanged();
}

bool KatalogueSettings::scannerFollowSymlinks() const {
    return settings().value(QStringLiteral("scanner/followSymlinks"), false).toBool();
}

void KatalogueSettings::setScannerFollowSymlinks(bool value) {
    settings().setValue(QStringLiteral("scanner/followSymlinks"), value);
    emit scannerSettingsChanged();
}

bool KatalogueSettings::scannerComputeHashes() const {
    return settings().value(QStringLiteral("scanner/computeHashes"), false).toBool();
}

void KatalogueSettings::setScannerComputeHashes(bool value) {
    settings().setValue(QStringLiteral("scanner/computeHashes"), value);
    emit scannerSettingsChanged();
}

int KatalogueSettings::scannerMaxDepth() const {
    return settings().value(QStringLiteral("scanner/maxDepth"), -1).toInt();
}

void KatalogueSettings::setScannerMaxDepth(int depth) {
    settings().setValue(QStringLiteral("scanner/maxDepth"), depth);
    emit scannerSettingsChanged();
}

QStringList KatalogueSettings::scannerExcludePatterns() const {
    return settings().value(QStringLiteral("scanner/excludePatterns"), QStringList()).toStringList();
}

void KatalogueSettings::setScannerExcludePatterns(const QStringList &patterns) {
    settings().setValue(QStringLiteral("scanner/excludePatterns"), patterns);
    emit scannerSettingsChanged();
}

bool KatalogueSettings::uiConfirmVirtualFolderDelete() const {
    return settings().value(QStringLiteral("ui/confirmVirtualFolderDelete"), true).toBool();
}

void KatalogueSettings::setUiConfirmVirtualFolderDelete(bool value) {
    settings().setValue(QStringLiteral("ui/confirmVirtualFolderDelete"), value);
    emit uiSettingsChanged();
}
