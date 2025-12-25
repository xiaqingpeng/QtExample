QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 设置macOS部署目标版本为26.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 26.0

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
    layoutexamples1/layoutexamplestab1.cpp \
    layoutexamples2/layoutexamplestab2.cpp \
    layoutexamples3/layoutexamplestab3.cpp \
    echarts/echartstab.cpp

HEADERS += \
    mainuiwindow.h \
    basiccontrols/basiccontrolstab.h \
    advancedcontrols/advancedcontrolstab.h \
    datadisplay/datadisplaytab.h \
    dialogs/dialogstab.h \
    layoutexamples1/layoutexamplestab1.h \
    layoutexamples2/layoutexamplestab2.h \
    layoutexamples3/layoutexamplestab3.h \
    echarts/echartstab.h

FORMS += \
    mainuiwindow.ui

# 添加资源文件
RESOURCES += layoutexamplestab.qrc

# 头文件路径
INCLUDEPATH += ./layoutexamples1 ./layoutexamples2 ./layoutexamples3

# 默认部署规则
qnx: target.path = /tmp/${TARGET}/bin
else: unix:!android: target.path = /opt/${TARGET}/bin
!isEmpty(target.path): INSTALLS += target