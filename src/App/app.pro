QT += core gui widgets webenginewidgets network

CONFIG += c++17
CONFIG -= single_module

# 启用企业级架构
DEFINES += ENTERPRISE_EDITION

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

# 创建应用程序
TEMPLATE = app
TARGET = "QT Enterprise Example"

# 设置应用程序图标
macx {
    ICON = ../../images/icon.png
    QMAKE_INFO_PLIST = ../../Info.plist.in
}

SOURCES += \
    main.cpp

HEADERS += \
    common.h

# 链接所有模块库
LIBS += -L$$OUT_PWD/../Core -lenterprisecore
LIBS += -L$$OUT_PWD/../Interfaces -lenterpriseinterfaces
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices
LIBS += -L$$OUT_PWD/../Localization -lenterpriselocalization
LIBS += -L$$OUT_PWD/../Utils -lutils
LIBS += -L$$OUT_PWD/../Styles -lstyles
LIBS += -L$$OUT_PWD/../Network -lnetwork
LIBS += -L$$OUT_PWD/../Analytics -lanalytics
LIBS += -L$$OUT_PWD/../Auth -lauth
LIBS += -L$$OUT_PWD/../UserProfile -luserprofile
LIBS += -L$$OUT_PWD/../Charts -lcharts
LIBS += -L$$OUT_PWD/../Reports -lreports
LIBS += -L$$OUT_PWD/../UI -lui

# 包含所有模块头文件路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core
INCLUDEPATH += ../Interfaces
INCLUDEPATH += ../Services
INCLUDEPATH += ../Localization
INCLUDEPATH += ../Utils
INCLUDEPATH += ../Styles
INCLUDEPATH += ../Network
INCLUDEPATH += ../Analytics
INCLUDEPATH += ../Auth
INCLUDEPATH += ../UserProfile
INCLUDEPATH += ../Charts
INCLUDEPATH += ../Reports
INCLUDEPATH += ../UI

# 包含资源文件
RESOURCES += ../../layoutexamplestab.qrc