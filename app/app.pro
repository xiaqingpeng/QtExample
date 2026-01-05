# 主应用程序的qmake项目文件

QT += core gui webenginewidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 启用企业级架构
DEFINES += ENTERPRISE_EDITION

# 禁用过时的single_module链接器标志
CONFIG -= single_module

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

# 创建应用程序
TEMPLATE = app
TARGET = "QT Example"

# 设置应用程序图标
macx {
    ICON = ../images/icon.png
    QMAKE_INFO_PLIST = ../Info.plist.in
}

SOURCES += \
    ../main.cpp \
    ../mainuiwindow.cpp

HEADERS += \
    ../mainuiwindow.h

FORMS += \
    ../mainuiwindow.ui

# 包含项目根目录以访问networkmanager.h
INCLUDEPATH += ..

# 链接企业级模块
LIBS += -L$$OUT_PWD/../src/Core -lenterprisecore
LIBS += -L$$OUT_PWD/../src/Interfaces -lenterpriseinterfaces
LIBS += -L$$OUT_PWD/../src/Services -lenterpriseservices
LIBS += -L$$OUT_PWD/../src/Localization -lenterpriselocalization

# 链接loginpage、changepasswordpage和userinfo模块
LIBS += -L$$OUT_PWD/../loginpage -lloginpage
LIBS += -L$$OUT_PWD/../changepasswordpage -lchangepasswordpage
LIBS += -L$$OUT_PWD/../userinfo -luserinfo

# 链接networkmanager模块
LIBS += -L$$OUT_PWD/../networkmanager -lnetworkmanager

# 链接analytics模块
LIBS += -L$$OUT_PWD/../analytics -lanalytics

# 链接Device模块
LIBS += -L$$OUT_PWD/../src/Device -ldevice

# 链接保留的模块
LIBS += -L$$OUT_PWD/../echarts -lecharts
LIBS += -L$$OUT_PWD/../reportstab -lreportstab
LIBS += -L$$OUT_PWD/../userprofiletab -luserprofiletab
LIBS += -L$$OUT_PWD/../styles -lstyles

# 包含企业级模块头文件路径
INCLUDEPATH += ../src
INCLUDEPATH += ../src/Core
INCLUDEPATH += ../src/Interfaces
INCLUDEPATH += ../src/Services
INCLUDEPATH += ../src/Localization

# 包含模块头文件路径
INCLUDEPATH += ../loginpage
INCLUDEPATH += ../changepasswordpage
INCLUDEPATH += ../userinfo
INCLUDEPATH += ../networkmanager
INCLUDEPATH += ../analytics
INCLUDEPATH += ../echarts
INCLUDEPATH += ../reportstab
INCLUDEPATH += ../userprofiletab
INCLUDEPATH += ../styles
INCLUDEPATH += ../src/Device

# 包含资源文件
RESOURCES += ../layoutexamplestab.qrc