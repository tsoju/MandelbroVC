/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      26,   11,   11,   11, 0x08,
      40,   11,   11,   11, 0x08,
      54,   11,   11,   11, 0x08,
      69,   11,   11,   11, 0x08,
      81,   11,   11,   11, 0x08,
      95,   11,   11,   11, 0x08,
     114,   11,   11,   11, 0x08,
     134,   11,   11,   11, 0x08,
     151,   11,   11,   11, 0x08,
     170,   11,   11,   11, 0x08,
     184,   11,   11,   11, 0x08,
     202,   11,   11,   11, 0x08,
     222,   11,   11,   11, 0x08,
     244,   11,   11,   11, 0x08,
     266,   11,   11,   11, 0x08,
     285,   11,   11,   11, 0x08,
     298,   11,   11,   11, 0x08,
     307,   11,   11,   11, 0x08,
     315,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0button_zoom()\0button_wide()\0"
    "button_left()\0button_right()\0button_up()\0"
    "button_down()\0button_left_step()\0"
    "button_right_step()\0button_up_step()\0"
    "button_down_step()\0clean_close()\0"
    "button_lupex_up()\0button_lupex_down()\0"
    "check_scale_show(int)\0show_version_dialog()\0"
    "check_for_update()\0fullscreen()\0"
    "redraw()\0go_to()\0replyFinished(QNetworkReply*)\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->button_zoom(); break;
        case 1: _t->button_wide(); break;
        case 2: _t->button_left(); break;
        case 3: _t->button_right(); break;
        case 4: _t->button_up(); break;
        case 5: _t->button_down(); break;
        case 6: _t->button_left_step(); break;
        case 7: _t->button_right_step(); break;
        case 8: _t->button_up_step(); break;
        case 9: _t->button_down_step(); break;
        case 10: _t->clean_close(); break;
        case 11: _t->button_lupex_up(); break;
        case 12: _t->button_lupex_down(); break;
        case 13: _t->check_scale_show((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->show_version_dialog(); break;
        case 15: _t->check_for_update(); break;
        case 16: _t->fullscreen(); break;
        case 17: _t->redraw(); break;
        case 18: _t->go_to(); break;
        case 19: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
