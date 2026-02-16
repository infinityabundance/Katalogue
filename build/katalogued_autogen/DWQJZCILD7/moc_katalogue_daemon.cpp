/****************************************************************************
** Meta object code from reading C++ file 'katalogue_daemon.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../katalogue/src/daemon/katalogue_daemon.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'katalogue_daemon.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15KatalogueDaemonE_t {};
} // unnamed namespace

template <> constexpr inline auto KatalogueDaemon::qt_create_metaobjectdata<qt_meta_tag_ZN15KatalogueDaemonE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "KatalogueDaemon",
        "D-Bus Interface",
        "org.kde.Katalogue1",
        "ScanProgress",
        "",
        "scanId",
        "path",
        "directories",
        "files",
        "bytes",
        "ScanFinished",
        "status",
        "Ping",
        "OpenProject",
        "QVariantMap",
        "GetProjectInfo",
        "StartScan",
        "rootPath",
        "CancelScan",
        "GetScanStatus",
        "ListVolumes",
        "ListDirectories",
        "QList<QVariantMap>",
        "volumeId",
        "parentId",
        "ListFiles",
        "directoryId",
        "SearchByName",
        "query",
        "limit",
        "offset",
        "Search",
        "fileType",
        "GetFileNote",
        "fileId",
        "SetFileNote",
        "content",
        "GetFileTags",
        "AddFileTag",
        "key",
        "value",
        "RemoveFileTag",
        "ListVirtualFolders",
        "CreateVirtualFolder",
        "name",
        "RenameVirtualFolder",
        "folderId",
        "newName",
        "DeleteVirtualFolder",
        "ListVirtualFolderItems",
        "AddFileToVirtualFolder",
        "RemoveFileFromVirtualFolder",
        "RenameVolume",
        "newLabel"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'ScanProgress'
        QtMocHelpers::SignalData<void(uint, const QString &, int, int, qint64)>(3, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 5 }, { QMetaType::QString, 6 }, { QMetaType::Int, 7 }, { QMetaType::Int, 8 },
            { QMetaType::LongLong, 9 },
        }}),
        // Signal 'ScanFinished'
        QtMocHelpers::SignalData<void(uint, const QString &)>(10, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 5 }, { QMetaType::QString, 11 },
        }}),
        // Slot 'Ping'
        QtMocHelpers::SlotData<QString() const>(12, 4, QMC::AccessPublic, QMetaType::QString),
        // Slot 'OpenProject'
        QtMocHelpers::SlotData<QVariantMap(const QString &)>(13, 4, QMC::AccessPublic, 0x80000000 | 14, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'GetProjectInfo'
        QtMocHelpers::SlotData<QVariantMap() const>(15, 4, QMC::AccessPublic, 0x80000000 | 14),
        // Slot 'StartScan'
        QtMocHelpers::SlotData<uint(const QString &)>(16, 4, QMC::AccessPublic, QMetaType::UInt, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'CancelScan'
        QtMocHelpers::SlotData<bool(uint)>(18, 4, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::UInt, 5 },
        }}),
        // Slot 'GetScanStatus'
        QtMocHelpers::SlotData<QVariantMap(uint) const>(19, 4, QMC::AccessPublic, 0x80000000 | 14, {{
            { QMetaType::UInt, 5 },
        }}),
        // Slot 'ListVolumes'
        QtMocHelpers::SlotData<QVariantMap() const>(20, 4, QMC::AccessPublic, 0x80000000 | 14),
        // Slot 'ListDirectories'
        QtMocHelpers::SlotData<QList<QVariantMap>(int, int) const>(21, 4, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::Int, 23 }, { QMetaType::Int, 24 },
        }}),
        // Slot 'ListFiles'
        QtMocHelpers::SlotData<QList<QVariantMap>(int) const>(25, 4, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::Int, 26 },
        }}),
        // Slot 'SearchByName'
        QtMocHelpers::SlotData<QVariantMap(const QString &, int, int) const>(27, 4, QMC::AccessPublic, 0x80000000 | 14, {{
            { QMetaType::QString, 28 }, { QMetaType::Int, 29 }, { QMetaType::Int, 30 },
        }}),
        // Slot 'Search'
        QtMocHelpers::SlotData<QList<QVariantMap>(const QString &, int, const QString &, int, int) const>(31, 4, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::QString, 28 }, { QMetaType::Int, 23 }, { QMetaType::QString, 32 }, { QMetaType::Int, 29 },
            { QMetaType::Int, 30 },
        }}),
        // Slot 'GetFileNote'
        QtMocHelpers::SlotData<QString(int) const>(33, 4, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 34 },
        }}),
        // Slot 'SetFileNote'
        QtMocHelpers::SlotData<void(int, const QString &)>(35, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 34 }, { QMetaType::QString, 36 },
        }}),
        // Slot 'GetFileTags'
        QtMocHelpers::SlotData<QList<QVariantMap>(int) const>(37, 4, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::Int, 34 },
        }}),
        // Slot 'AddFileTag'
        QtMocHelpers::SlotData<void(int, const QString &, const QString &)>(38, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 34 }, { QMetaType::QString, 39 }, { QMetaType::QString, 40 },
        }}),
        // Slot 'RemoveFileTag'
        QtMocHelpers::SlotData<void(int, const QString &, const QString &)>(41, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 34 }, { QMetaType::QString, 39 }, { QMetaType::QString, 40 },
        }}),
        // Slot 'ListVirtualFolders'
        QtMocHelpers::SlotData<QList<QVariantMap>(int) const>(42, 4, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::Int, 24 },
        }}),
        // Slot 'CreateVirtualFolder'
        QtMocHelpers::SlotData<int(const QString &, int)>(43, 4, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::QString, 44 }, { QMetaType::Int, 24 },
        }}),
        // Slot 'RenameVirtualFolder'
        QtMocHelpers::SlotData<void(int, const QString &)>(45, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 46 }, { QMetaType::QString, 47 },
        }}),
        // Slot 'DeleteVirtualFolder'
        QtMocHelpers::SlotData<void(int)>(48, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 46 },
        }}),
        // Slot 'ListVirtualFolderItems'
        QtMocHelpers::SlotData<QList<QVariantMap>(int) const>(49, 4, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::Int, 46 },
        }}),
        // Slot 'AddFileToVirtualFolder'
        QtMocHelpers::SlotData<void(int, int)>(50, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 46 }, { QMetaType::Int, 34 },
        }}),
        // Slot 'RemoveFileFromVirtualFolder'
        QtMocHelpers::SlotData<void(int, int)>(51, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 46 }, { QMetaType::Int, 34 },
        }}),
        // Slot 'RenameVolume'
        QtMocHelpers::SlotData<void(int, const QString &)>(52, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 23 }, { QMetaType::QString, 53 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
    });
    return QtMocHelpers::metaObjectData<KatalogueDaemon, qt_meta_tag_ZN15KatalogueDaemonE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject KatalogueDaemon::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15KatalogueDaemonE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15KatalogueDaemonE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15KatalogueDaemonE_t>.metaTypes,
    nullptr
} };

void KatalogueDaemon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<KatalogueDaemon *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->ScanProgress((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 1: _t->ScanFinished((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: { QString _r = _t->Ping();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QVariantMap _r = _t->OpenProject((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 4: { QVariantMap _r = _t->GetProjectInfo();
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 5: { uint _r = _t->StartScan((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<uint*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->CancelScan((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { QVariantMap _r = _t->GetScanStatus((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 8: { QVariantMap _r = _t->ListVolumes();
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 9: { QList<QVariantMap> _r = _t->ListDirectories((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QList<QVariantMap>*>(_a[0]) = std::move(_r); }  break;
        case 10: { QList<QVariantMap> _r = _t->ListFiles((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QList<QVariantMap>*>(_a[0]) = std::move(_r); }  break;
        case 11: { QVariantMap _r = _t->SearchByName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 12: { QList<QVariantMap> _r = _t->Search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5])));
            if (_a[0]) *reinterpret_cast<QList<QVariantMap>*>(_a[0]) = std::move(_r); }  break;
        case 13: { QString _r = _t->GetFileNote((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 14: _t->SetFileNote((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 15: { QList<QVariantMap> _r = _t->GetFileTags((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QList<QVariantMap>*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->AddFileTag((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 17: _t->RemoveFileTag((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 18: { QList<QVariantMap> _r = _t->ListVirtualFolders((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QList<QVariantMap>*>(_a[0]) = std::move(_r); }  break;
        case 19: { int _r = _t->CreateVirtualFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 20: _t->RenameVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 21: _t->DeleteVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 22: { QList<QVariantMap> _r = _t->ListVirtualFolderItems((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QList<QVariantMap>*>(_a[0]) = std::move(_r); }  break;
        case 23: _t->AddFileToVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 24: _t->RemoveFileFromVirtualFolder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 25: _t->RenameVolume((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (KatalogueDaemon::*)(uint , const QString & , int , int , qint64 )>(_a, &KatalogueDaemon::ScanProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (KatalogueDaemon::*)(uint , const QString & )>(_a, &KatalogueDaemon::ScanFinished, 1))
            return;
    }
}

const QMetaObject *KatalogueDaemon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KatalogueDaemon::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15KatalogueDaemonE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QDBusContext"))
        return static_cast< QDBusContext*>(this);
    return QObject::qt_metacast(_clname);
}

int KatalogueDaemon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 26;
    }
    return _id;
}

// SIGNAL 0
void KatalogueDaemon::ScanProgress(uint _t1, const QString & _t2, int _t3, int _t4, qint64 _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 1
void KatalogueDaemon::ScanFinished(uint _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
QT_WARNING_POP
