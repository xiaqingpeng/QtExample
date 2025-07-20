QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 你可以让代码在使用已弃用API时报错
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    mainuiwindow.cpp \
    basiccontrolstab.cpp \
    datadisplaytab.cpp \
    advancedcontrolstab.cpp \
    dialogstab.cpp \
    layoutexamplestab.cpp

HEADERS += \
    mainuiwindow.h \
    basiccontrolstab.h \
    datadisplaytab.h \
    advancedcontrolstab.h \
    dialogstab.h \
    layoutexamplestab.h

FORMS += \
    mainuiwindow.ui

RESOURCES += resources.qrc

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
