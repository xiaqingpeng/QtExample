QT       += core gui webenginewidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 设置macOS部署目标版本为26.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 26.0

# 你可以让代码在使用已弃用API时报错
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

TEMPLATE = subdirs

# 子项目列表
SUBDIRS += \
    loginpage \
    changepasswordpage \
    userinfo \
    userprofiletab \
    reportstab \
    networkmanager \
    analytics \
    advancedcontrols \
    basiccontrols \
    datadisplay \
    dialogs \
    echarts \
    layoutexamples1 \
    layoutexamples2/layoutexamplestab2.pro \
    layoutexamples3/layoutexamplestab3.pro \
    app

# 设置app依赖所有库
app.depends = loginpage changepasswordpage userinfo userprofiletab reportstab networkmanager analytics advancedcontrols basiccontrols datadisplay dialogs echarts layoutexamples1 layoutexamples2/layoutexamplestab2.pro layoutexamples3/layoutexamplestab3.pro