TEMPLATE = subdirs
CONFIG += ordered sdk_no_version_check

# Qt大型项目模块结构
SUBDIRS += \
    Interfaces \
    Localization \
    Services \
    Core \
    Utils \
    Styles \
    Network \
    Analytics \
    Auth \
    UserProfile \
    Charts \
    Reports \
    Device \
    UI \
    App

# 设置依赖关系
# 基础层
Services.depends = Interfaces
Core.depends = Localization Services

# 工具层
Utils.depends = Core
Styles.depends = Core Utils

# 业务层
Network.depends = Core Services
Analytics.depends = Core Services Network
Auth.depends = Core Services Network Styles
UserProfile.depends = Core Services Network Auth Styles
Charts.depends = Core Services Network Styles
Reports.depends = Core Services Network Analytics Styles
Device.depends = Core Services Network Styles

# UI层
UI.depends = Core Services Network Analytics Auth UserProfile Charts Reports Device Styles Utils

# 应用层
App.depends = UI