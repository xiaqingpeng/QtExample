QT += core gui widgets

CONFIG += c++17

# 定义源文件
SOURCES += \
    theme_manager.cpp

# 定义头文件
HEADERS += \
    theme_manager.h

# 定义库文件 - 使用静态库
TEMPLATE = lib
CONFIG += staticlib

# 设置目标名称
TARGET = styles

# macOS 特定设置
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0
}

# 设置输出目录
DESTDIR = $$OUT_PWD