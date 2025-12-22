QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 你可以让代码在使用已弃用API时报错
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

TEMPLATE = app

SOURCES += \
    main.cpp \
    mainuiwindow.cpp \
    basiccontrols/basiccontrolstab.cpp \
    advancedcontrols/advancedcontrolstab.cpp \
    datadisplay/datadisplaytab.cpp \
    dialogs/dialogstab.cpp \
    layoutexamples/layoutexamplestab.cpp \
    echarts/echartstab.cpp

HEADERS += \
    mainuiwindow.h \
    basiccontrols/basiccontrolstab.h \
    advancedcontrols/advancedcontrolstab.h \
    datadisplay/datadisplaytab.h \
    dialogs/dialogstab.h \
    layoutexamples/layoutexamplestab.h \
    echarts/echartstab.h

FORMS += \
    mainuiwindow.ui

# 添加资源文件
RESOURCES += layoutexamplestab.qrc

# 默认部署规则
qnx: target.path = /tmp/${TARGET}/bin
else: unix:!android: target.path = /opt/${TARGET}/bin
!isEmpty(target.path): INSTALLS += target