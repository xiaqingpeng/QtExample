# LoginPage模块的qmake项目文件

QT += core gui widgets network

CONFIG += c++17 sdk_no_version_check

# 创建共享库
TEMPLATE = lib
CONFIG += shared

TARGET = loginpage

SOURCES += \
    loginpage.cpp \
    ../networkmanager.cpp \
    ../analytics/analytics.cpp

HEADERS += \
    loginpage.h \
    ../networkmanager.h \
    ../analytics/analytics.h

# 包含项目根目录以访问networkmanager.h和analytics.h
INCLUDEPATH += .. ../analytics ../styles

# 链接styles库
LIBS += -L$$OUT_PWD/../styles -lstyles

# 安装配置
target.path = $[QT_INSTALL_LIBS]
INSTALLS += target

# 头文件安装
headers.files = $HEADERS
headers.path = $[QT_INSTALL_HEADERS]/loginpage
INSTALLS += headers