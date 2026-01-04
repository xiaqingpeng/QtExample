QT       += core gui webenginewidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 启用企业级架构
DEFINES += ENTERPRISE_EDITION

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

# 你可以让代码在使用已弃用API时报错
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

TEMPLATE = subdirs

# 子项目列表
SUBDIRS += \
    src \
    networkmanager \
    analytics \
    loginpage \
    changepasswordpage \
    userinfo \
    echarts \
    reportstab \
    userprofiletab \
    styles \
    app

# 设置依赖关系 - 确保styles库先于其他模块构建
changepasswordpage.depends = styles
userinfo.depends = styles
echarts.depends = networkmanager styles
reportstab.depends = styles
userprofiletab.depends = styles

# 设置app依赖所有库，包括企业级模块
app.depends = src loginpage changepasswordpage userinfo networkmanager analytics echarts reportstab userprofiletab styles