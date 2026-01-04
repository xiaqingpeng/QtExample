TEMPLATE = subdirs

# 企业级模块子项目
SUBDIRS += \
    Interfaces \
    Localization \
    Services \
    Core

# 设置依赖关系
Services.depends = Interfaces
Core.depends = Localization Services