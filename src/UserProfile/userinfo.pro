# UserInfo模块的qmake项目文件

QT += core gui widgets network

CONFIG += c++17 sdk_no_version_check

# 创建共享库
TEMPLATE = lib
CONFIG += shared

TARGET = userinfo

SOURCES += \
    userinfopage.cpp

HEADERS += \
    userinfopage.h

# 包含路径
INCLUDEPATH += .. ../analytics ../styles ../Services

# 链接Services库
LIBS += -L$$OUT_PWD/../Services -lenterpriseservices

# 链接Analytics库
LIBS += -L$$OUT_PWD/../Analytics -lanalytics

# 链接styles库 (静态库) - 使用PRE_TARGETDEPS确保构建顺序
PRE_TARGETDEPS += $OUT_PWD/../styles/libstyles.a
LIBS += -L$$OUT_PWD/../styles -lstyles

# 安装配置
target.path = $[QT_INSTALL_LIBS]
INSTALLS += target

# 头文件安装
headers.files = $HEADERS
headers.path = $[QT_INSTALL_HEADERS]/userinfo
INSTALLS += headers