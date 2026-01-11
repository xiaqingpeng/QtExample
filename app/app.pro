# 主应用程序的qmake项目文件

QT += core gui webenginewidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

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

# 链接loginpage、changepasswordpage和userinfo模块
LIBS += -L$$OUT_PWD/../loginpage -lloginpage
LIBS += -L$$OUT_PWD/../changepasswordpage -lchangepasswordpage
LIBS += -L$$OUT_PWD/../userinfo -luserinfo

# 链接networkmanager模块
LIBS += -L$$OUT_PWD/../networkmanager -lnetworkmanager

# 链接analytics模块
LIBS += -L$$OUT_PWD/../analytics -lanalytics

# 链接其他模块（使用正确的库名称）
LIBS += -L$$OUT_PWD/../advancedcontrols -ladvancedcontrols
LIBS += -L$$OUT_PWD/../basiccontrols -lbasiccontrols
LIBS += -L$$OUT_PWD/../datadisplay -ldatadisplay
LIBS += -L$$OUT_PWD/../dialogs -ldialogs
LIBS += -L$$OUT_PWD/../layoutexamples1 -lLayoutExamples1
LIBS += -L$$OUT_PWD/../layoutexamples2 -llayoutexamplestab2
LIBS += -L$$OUT_PWD/../layoutexamples3 -llayoutexamplestab3
LIBS += -L$$OUT_PWD/../reportstab -lreportstab
LIBS += -L$$OUT_PWD/../userprofiletab -luserprofiletab
LIBS += -L$$OUT_PWD/../echarts -lecharts
LIBS += -L$$OUT_PWD/../styles -lstyles

# 包含模块头文件路径
INCLUDEPATH += ../loginpage
INCLUDEPATH += ../changepasswordpage
INCLUDEPATH += ../userinfo
INCLUDEPATH += ../networkmanager
INCLUDEPATH += ../analytics
INCLUDEPATH += ../advancedcontrols
INCLUDEPATH += ../basiccontrols
INCLUDEPATH += ../datadisplay
INCLUDEPATH += ../dialogs
INCLUDEPATH += ../layoutexamples1
INCLUDEPATH += ../layoutexamples2
INCLUDEPATH += ../layoutexamples3
INCLUDEPATH += ../reportstab
INCLUDEPATH += ../userprofiletab
INCLUDEPATH += ../echarts
INCLUDEPATH += ../styles

# 包含资源文件
RESOURCES += ../layoutexamplestab.qrc