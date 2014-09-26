# -------------------------------------------------
# Project created by QtCreator 2010-05-21T22:37:21
# -------------------------------------------------
TARGET = JsonEditor

QT       += core gui multimedia multimediawidgets widgets

ICON = icon.icns
RC_FILE = myapp.rc

TEMPLATE = app
SOURCES += main.cpp \
    jsoneditormain.cpp \
    jsonc/printbuf.c \
    jsonc/linkhash.c \
    jsonc/jsoncxx.cpp \
    jsonc/json_util.c \
    jsonc/json_tokener.c \
    jsonc/json_object.c \
    jsonc/debug.c \
    jsonc/arraylist.c \
    jsontreeitem.cpp \
    jsontreemodel.cpp \
    addnodedlg.cpp \
    jsedit.cpp
HEADERS += jsoneditormain.h \
    jsonc/printbuf.h \
    jsonc/linkhash.h \
    jsonc/jsoncxx.h \
    jsonc/json_util.h \
    jsonc/json_tokener.h \
    jsonc/json_object_private.h \
    jsonc/json_object.h \
    jsonc/json.h \
    jsonc/debug.h \
    jsonc/bits.h \
    jsonc/arraylist.h \
    jsonc/config.h \
    jsontreeitem.h \
    jsontreemodel.h \
    addnodedlg.h \
    version.h \
    jsedit.h \
    JSHighlighter.h
FORMS += jsoneditormain.ui \
    addnodedlg.ui
OTHER_FILES += images/view-refresh.png \
    images/system-log-out.png \
    images/media-floppy.png \
    images/format-justify-right.png \
    images/folder-open.png \
    images/edit-undo.png \
    images/edit-redo.png \
    images/edit-paste.png \
    images/edit-find-replace.png \
    images/edit-find.png \
    images/edit-cut.png \
    images/edit-copy.png \
    images/edit-clear.png \
    images/document-save.png \
    images/document-open.png \
    myapp.rc \
    images/document-new.png
RESOURCES += jsoneditor.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/ -lqtfindreplacedialog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ -lqtfindreplacedialogd
else:unix: LIBS += -L$$PWD/libs/ -lqtfindreplacedialog

INCLUDEPATH += $$PWD/dialogs
DEPENDPATH += $$PWD/dialogs

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/libqtfindreplacedialog.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/libqtfindreplacedialogd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/qtfindreplacedialog.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/qtfindreplacedialogd.lib
else:unix: PRE_TARGETDEPS += $$PWD/libs/libqtfindreplacedialog.a
