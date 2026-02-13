QT += core gui widgets network charts

CONFIG += c++17 staticlib sdk_no_version_check
CONFIG -= single_module

TEMPLATE = lib
TARGET = device

HEADERS += \
    serverconfigtab.h \
    contenttab.h

SOURCES += \
    serverconfigtab.cpp \
    contenttab.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Services
INCLUDEPATH += ../Styles

# 链接Services库
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices

# 链接Charts库（包含GaugeWidget）
LIBS += -L$$OUT_PWD/../Charts -lcharts

# 资源文件
RESOURCES += ../../layoutexamplestab.qrc