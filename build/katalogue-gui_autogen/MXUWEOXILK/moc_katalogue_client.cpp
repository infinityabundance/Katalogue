/****************************************************************************
** Meta object code from reading C++ file 'katalogue_client.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../katalogue/src/gui/katalogue_client.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'katalogue_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15KatalogueClientE_t {};
} // unnamed namespace

template <> constexpr inline auto KatalogueClient::qt_create_metaobjectdata<qt_meta_tag_ZN15KatalogueClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "KatalogueClient",
        "volumesChanged",
        "",
        "searchResultsChanged",
        "selectedVolumeIdChanged",
        "selectedDirectoryIdChanged",
        "selectedFileIdChanged",
        "settingsChanged",
        "directoryEntriesChanged",
        "fileEntriesChanged",
        "activeScansChanged",
        "projectPathChanged",
        "projectInfoChanged",
        "virtualFoldersChanged",
        "selectedVirtualFolderIdChanged",
        "virtualFolderItemsChanged",
        "scanProgress",
        "scanId",
        "path",
        "directories",
        "files",
        "bytes",
        "scanFinished",
        "status",
        "onScanProgress",
        "onScanFinished",
        "openProject",
        "startScan",
        "rootPath",
        "cancelScan",
        "scanStatus",
        "QVariantMap",
        "refreshVolumes",
        "searchByName",
        "query",
        "limit",
        "offset",
        "search",
        "volumeId",
        "fileType",
        "jumpToResult",
        "directoryId",
        "getFileNote",
        "fileId",
        "setFileNote",
        "content",
        "getFileTags",
        "QVariantList",
        "addFileTag",
        "key",
        "value",
        "removeFileTag",
        "loadVirtualFolderChildren",
        "parentId",
        "createVirtualFolder",
        "name",
        "renameVirtualFolder",
        "folderId",
        "newName",
        "deleteVirtualFolder",
        "loadVirtualFolderItems",
        "addFileToVirtualFolder",
        "removeFileFromVirtualFolder",
        "jumpToVirtualFolderItem",
        "renameVolume",
        "newLabel",
        "copyTextToClipboard",
        "text",
        "getFileInfo",
        "refreshProjectInfo",
        "loadRootForVolume",
        "loadDirectory",
        "volumes",
        "searchResults",
        "selectedVolumeId",
        "selectedDirectoryId",
        "selectedFileId",
        "directoryEntries",
        "fileEntries",
        "activeScans",
        "projectPath",
        "projectInfo",
        "virtualFolders",
        "selectedVirtualFolderId",
        "virtualFolderItems",
        "scannerIncludeHidden",
        "scannerFollowSymlinks",
        "scannerComputeHashes",
        "scannerMaxDepth",
        "scannerExcludePatternsString",
        "uiConfirmVirtualFolderDelete",
        "appVersion"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'volumesChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchResultsChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedVolumeIdChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedDirectoryIdChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedFileIdChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'settingsChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'directoryEntriesChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fileEntriesChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'activeScansChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'projectPathChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'projectInfoChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'virtualFoldersChanged'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedVirtualFolderIdChanged'
        QtMocHelpers::SignalData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'virtualFolderItemsChanged'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'scanProgress'
        QtMocHelpers::SignalData<void(uint, const QString &, int, int, qint64)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 17 }, { QMetaType::QString, 18 }, { QMetaType::Int, 19 }, { QMetaType::Int, 20 },
            { QMetaType::LongLong, 21 },
        }}),
        // Signal 'scanFinished'
        QtMocHelpers::SignalData<void(uint, const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 17 }, { QMetaType::QString, 23 },
        }}),
        // Slot 'onScanProgress'
        QtMocHelpers::SlotData<void(uint, const QString &, int, int, qint64)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 17 }, { QMetaType::QString, 18 }, { QMetaType::Int, 19 }, { QMetaType::Int, 20 },
            { QMetaType::LongLong, 21 },
        }}),
        // Slot 'onScanFinished'
        QtMocHelpers::SlotData<void(uint, const QString &)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 17 }, { QMetaType::QString, 23 },
        }}),
        // Method 'openProject'
        QtMocHelpers::MethodData<void(const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'startScan'
        QtMocHelpers::MethodData<int(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::QString, 28 },
        }}),
        // Method 'cancelScan'
        QtMocHelpers::MethodData<void(int)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 17 },
        }}),
        // Method 'scanStatus'
        QtMocHelpers::MethodData<QVariantMap(int) const>(30, 2, QMC::AccessPublic, 0x80000000 | 31, {{
            { QMetaType::Int, 17 },
        }}),
        // Method 'refreshVolumes'
        QtMocHelpers::MethodData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'searchByName'
        QtMocHelpers::MethodData<void(const QString &, int, int)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Int, 35 }, { QMetaType::Int, 36 },
        }}),
        // Method 'searchByName'
        QtMocHelpers::MethodData<void(const QString &, int)>(33, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Int, 35 },
        }}),
        // Method 'searchByName'
        QtMocHelpers::MethodData<void(const QString &)>(33, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Method 'search'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &, int, int)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Int, 38 }, { QMetaType::QString, 39 }, { QMetaType::Int, 35 },
            { QMetaType::Int, 36 },
        }}),
        // Method 'search'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &, int)>(37, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Int, 38 }, { QMetaType::QString, 39 }, { QMetaType::Int, 35 },
        }}),
        // Method 'search'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &)>(37, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Int, 38 }, { QMetaType::QString, 39 },
        }}),
        // Method 'search'
        QtMocHelpers::MethodData<void(const QString &, int)>(37, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Int, 38 },
        }}),
        // Method 'search'
        QtMocHelpers::MethodData<void(const QString &)>(37, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Method 'jumpToResult'
        QtMocHelpers::MethodData<void(int, int)>(40, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 38 }, { QMetaType::Int, 41 },
        }}),
        // Method 'getFileNote'
        QtMocHelpers::MethodData<QString(int)>(42, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 43 },
        }}),
        // Method 'setFileNote'
        QtMocHelpers::MethodData<void(int, const QString &)>(44, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 43 }, { QMetaType::QString, 45 },
        }}),
        // Method 'getFileTags'
        QtMocHelpers::MethodData<QVariantList(int)>(46, 2, QMC::AccessPublic, 0x80000000 | 47, {{
            { QMetaType::Int, 43 },
        }}),
        // Method 'addFileTag'
        QtMocHelpers::MethodData<void(int, const QString &, const QString &)>(48, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 43 }, { QMetaType::QString, 49 }, { QMetaType::QString, 50 },
        }}),
        // Method 'removeFileTag'
        QtMocHelpers::MethodData<void(int, const QString &, const QString &)>(51, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 43 }, { QMetaType::QString, 49 }, { QMetaType::QString, 50 },
        }}),
        // Method 'loadVirtualFolderChildren'
        QtMocHelpers::MethodData<void(int)>(52, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 53 },
        }}),
        // Method 'createVirtualFolder'
        QtMocHelpers::MethodData<int(const QString &, int)>(54, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::QString, 55 }, { QMetaType::Int, 53 },
        }}),
        // Method 'renameVirtualFolder'
        QtMocHelpers::MethodData<void(int, const QString &)>(56, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 57 }, { QMetaType::QString, 58 },
        }}),
        // Method 'deleteVirtualFolder'
        QtMocHelpers::MethodData<void(int)>(59, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 57 },
        }}),
        // Method 'loadVirtualFolderItems'
        QtMocHelpers::MethodData<void(int)>(60, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 57 },
        }}),
        // Method 'addFileToVirtualFolder'
        QtMocHelpers::MethodData<void(int, int)>(61, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 57 }, { QMetaType::Int, 43 },
        }}),
        // Method 'removeFileFromVirtualFolder'
        QtMocHelpers::MethodData<void(int, int)>(62, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 57 }, { QMetaType::Int, 43 },
        }}),
        // Method 'jumpToVirtualFolderItem'
        QtMocHelpers::MethodData<void(int, int)>(63, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 38 }, { QMetaType::Int, 41 },
        }}),
        // Method 'renameVolume'
        QtMocHelpers::MethodData<void(int, const QString &)>(64, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 38 }, { QMetaType::QString, 65 },
        }}),
        // Method 'copyTextToClipboard'
        QtMocHelpers::MethodData<void(const QString &)>(66, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 67 },
        }}),
        // Method 'getFileInfo'
        QtMocHelpers::MethodData<QVariantMap(int) const>(68, 2, QMC::AccessPublic, 0x80000000 | 31, {{
            { QMetaType::Int, 43 },
        }}),
        // Method 'refreshProjectInfo'
        QtMocHelpers::MethodData<void()>(69, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'loadRootForVolume'
        QtMocHelpers::MethodData<void(int)>(70, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 38 },
        }}),
        // Method 'loadDirectory'
        QtMocHelpers::MethodData<void(int)>(71, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 41 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'volumes'
        QtMocHelpers::PropertyData<QVariantList>(72, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'searchResults'
        QtMocHelpers::PropertyData<QVariantList>(73, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 1),
        // property 'selectedVolumeId'
        QtMocHelpers::PropertyData<int>(74, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'selectedDirectoryId'
        QtMocHelpers::PropertyData<int>(75, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'selectedFileId'
        QtMocHelpers::PropertyData<int>(76, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'directoryEntries'
        QtMocHelpers::PropertyData<QVariantList>(77, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 6),
        // property 'fileEntries'
        QtMocHelpers::PropertyData<QVariantList>(78, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 7),
        // property 'activeScans'
        QtMocHelpers::PropertyData<QVariantList>(79, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 8),
        // property 'projectPath'
        QtMocHelpers::PropertyData<QString>(80, QMetaType::QString, QMC::DefaultPropertyFlags, 9),
        // property 'projectInfo'
        QtMocHelpers::PropertyData<QVariantMap>(81, 0x80000000 | 31, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 10),
        // property 'virtualFolders'
        QtMocHelpers::PropertyData<QVariantList>(82, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 11),
        // property 'selectedVirtualFolderId'
        QtMocHelpers::PropertyData<int>(83, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 12),
        // property 'virtualFolderItems'
        QtMocHelpers::PropertyData<QVariantList>(84, 0x80000000 | 47, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 13),
        // property 'scannerIncludeHidden'
        QtMocHelpers::PropertyData<bool>(85, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'scannerFollowSymlinks'
        QtMocHelpers::PropertyData<bool>(86, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'scannerComputeHashes'
        QtMocHelpers::PropertyData<bool>(87, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'scannerMaxDepth'
        QtMocHelpers::PropertyData<int>(88, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'scannerExcludePatternsString'
        QtMocHelpers::PropertyData<QString>(89, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'uiConfirmVirtualFolderDelete'
        QtMocHelpers::PropertyData<bool>(90, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'appVersion'
        QtMocHelpers::PropertyData<QString>(91, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<KatalogueClient, qt_meta_tag_ZN15KatalogueClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject KatalogueClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15KatalogueClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15KatalogueClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15KatalogueClientE_t>.metaTypes,
    nullptr
} };

void KatalogueClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<KatalogueClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->volumesChanged(); break;
        case 1: _t->searchResultsChanged(); break;
        case 2: _t->selectedVolumeIdChanged(); break;
        case 3: _t->selectedDirectoryIdChanged(); break;
        case 4: _t->selectedFileIdChanged(); break;
        case 5: _t->settingsChanged(); break;
        case 6: _t->directoryEntriesChanged(); break;
        case 7: _t->fileEntriesChanged(); break;
        case 8: _t->activeScansChanged(); break;
        case 9: _t->projectPathChanged(); break;
        case 10: _t->projectInfoChanged(); break;
        case 11: _t->virtualFoldersChanged(); break;
        case 12: _t->selectedVirtualFolderIdChanged(); break;
        case 13: _t->virtualFolderItemsChanged(); break;
        case 14: _t->scanProgress((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 15: _t->scanFinished((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 16: _t->onScanProgress((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 17: _t->onScanFinished((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 18: _t->openProject((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: { int _r = _t->startScan((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 20: _t->cancelScan((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 21: { QVariantMap _r = _t->scanStatus((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 22: _t->refreshVolumes(); break;
        case 23: _t->searchByName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 24: _t->searchByName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 25: _t->searchByName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5]))); break;
        case 27: _t->search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4]))); break;
        case 28: _t->search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 29: _t->search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 30: _t->search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 31: _t->jumpToResult((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 32: { QString _r = _t->getFileNote((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 33: _t->setFileNote((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 34: { QVariantList _r = _t->getFileTags((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 35: _t->addFileTag((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 36: _t->removeFileTag((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 37: _t->loadVirtualFolderChildren((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 38: { int _r = _t->createVirtualFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 39: _t->renameVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 40: _t->deleteVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 41: _t->loadVirtualFolderItems((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 42: _t->addFileToVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 43: _t->removeFileFromVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 44: _t->jumpToVirtualFolderItem((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 45: _t->renameVolume((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 46: _t->copyTextToClipboard((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 47: { QVariantMap _r = _t->getFileInfo((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 48: _t->refreshProjectInfo(); break;
        case 49: _t->loadRootForVolume((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 50: _t->loadDirectory((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::volumesChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::searchResultsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::selectedVolumeIdChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::selectedDirectoryIdChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::selectedFileIdChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::settingsChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::directoryEntriesChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::fileEntriesChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::activeScansChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::projectPathChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::projectInfoChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::virtualFoldersChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::selectedVirtualFolderIdChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)()>(_a, &KatalogueClient::virtualFolderItemsChanged, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)(uint , const QString & , int , int , qint64 )>(_a, &KatalogueClient::scanProgress, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueClient::*)(uint , const QString & )>(_a, &KatalogueClient::scanFinished, 15))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QVariantList*>(_v) = _t->volumes(); break;
        case 1: *reinterpret_cast<QVariantList*>(_v) = _t->searchResults(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->selectedVolumeId(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->selectedDirectoryId(); break;
        case 4: *reinterpret_cast<int*>(_v) = _t->selectedFileId(); break;
        case 5: *reinterpret_cast<QVariantList*>(_v) = _t->directoryEntries(); break;
        case 6: *reinterpret_cast<QVariantList*>(_v) = _t->fileEntries(); break;
        case 7: *reinterpret_cast<QVariantList*>(_v) = _t->activeScans(); break;
        case 8: *reinterpret_cast<QString*>(_v) = _t->projectPath(); break;
        case 9: *reinterpret_cast<QVariantMap*>(_v) = _t->projectInfo(); break;
        case 10: *reinterpret_cast<QVariantList*>(_v) = _t->virtualFolders(); break;
        case 11: *reinterpret_cast<int*>(_v) = _t->selectedVirtualFolderId(); break;
        case 12: *reinterpret_cast<QVariantList*>(_v) = _t->virtualFolderItems(); break;
        case 13: *reinterpret_cast<bool*>(_v) = _t->scannerIncludeHidden(); break;
        case 14: *reinterpret_cast<bool*>(_v) = _t->scannerFollowSymlinks(); break;
        case 15: *reinterpret_cast<bool*>(_v) = _t->scannerComputeHashes(); break;
        case 16: *reinterpret_cast<int*>(_v) = _t->scannerMaxDepth(); break;
        case 17: *reinterpret_cast<QString*>(_v) = _t->scannerExcludePatternsString(); break;
        case 18: *reinterpret_cast<bool*>(_v) = _t->uiConfirmVirtualFolderDelete(); break;
        case 19: *reinterpret_cast<QString*>(_v) = _t->appVersion(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: _t->setSelectedVolumeId(*reinterpret_cast<int*>(_v)); break;
        case 3: _t->setSelectedDirectoryId(*reinterpret_cast<int*>(_v)); break;
        case 4: _t->setSelectedFileId(*reinterpret_cast<int*>(_v)); break;
        case 11: _t->setSelectedVirtualFolderId(*reinterpret_cast<int*>(_v)); break;
        case 13: _t->setScannerIncludeHidden(*reinterpret_cast<bool*>(_v)); break;
        case 14: _t->setScannerFollowSymlinks(*reinterpret_cast<bool*>(_v)); break;
        case 15: _t->setScannerComputeHashes(*reinterpret_cast<bool*>(_v)); break;
        case 16: _t->setScannerMaxDepth(*reinterpret_cast<int*>(_v)); break;
        case 17: _t->setScannerExcludePatternsString(*reinterpret_cast<QString*>(_v)); break;
        case 18: _t->setUiConfirmVirtualFolderDelete(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *KatalogueClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KatalogueClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15KatalogueClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KatalogueClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 51)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 51;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 51)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 51;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void KatalogueClient::volumesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KatalogueClient::searchResultsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KatalogueClient::selectedVolumeIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void KatalogueClient::selectedDirectoryIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void KatalogueClient::selectedFileIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void KatalogueClient::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void KatalogueClient::directoryEntriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void KatalogueClient::fileEntriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void KatalogueClient::activeScansChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void KatalogueClient::projectPathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void KatalogueClient::projectInfoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void KatalogueClient::virtualFoldersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void KatalogueClient::selectedVirtualFolderIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void KatalogueClient::virtualFolderItemsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void KatalogueClient::scanProgress(uint _t1, const QString & _t2, int _t3, int _t4, qint64 _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 15
void KatalogueClient::scanFinished(uint _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1, _t2);
}
QT_WARNING_POP
