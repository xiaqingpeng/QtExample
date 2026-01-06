QT       += core gui webenginewidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 启用企业级架构
DEFINES += ENTERPRISE_EDITION

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

TEMPLATE = subdirs
CONFIG += sdk_no_version_check

# Qt大型项目结构 - 只构建src目录
SUBDIRS += src

# src目录包含所有模块，按照依赖关系自动构建