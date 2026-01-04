QT += core

CONFIG += c++17 staticlib
CONFIG -= single_module

TEMPLATE = lib
TARGET = utils

HEADERS += \
    logger.h \
    avatar_utils.h

SOURCES += \
    logger.cpp \
    avatar_utils.cpp

# 包含路径
INCLUDEPATH += ../..
INCLUDEPATH += ../Core