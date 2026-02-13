QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 sdk_no_version_check

# 企业级架构作为标准（移除条件编译）
# DEFINES += ENTERPRISE_EDITION # 不再需要条件编译

# 设置macOS部署目标版本为14.0，与Qt库版本兼容
QMAKE_MACOSX_DEPLOYMENT_TARGET = 27.0

TEMPLATE = subdirs
CONFIG += sdk_no_version_check

# Qt大型项目结构 - 只构建src目录
SUBDIRS += src

# src目录包含所有模块，按照依赖关系自动构建