qt6_add_executable(QtEnterpriseApp
    main.cpp
    common.h
)

# 添加资源文件
qt6_add_resources(QtEnterpriseApp "resources"
    PREFIX "/"
    BASE "${CMAKE_SOURCE_DIR}/../.."
    FILES
        ../../layoutexamplestab.qrc
)

target_link_libraries(QtEnterpriseApp
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    enterprisecore
    enterpriseinterfaces
    enterpriseservices
    enterpriselocalization
    utils
    styles
    network
    analytics
    auth
    userprofile
    charts
    reports
    ui
)

target_include_directories(QtEnterpriseApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

# 设置应用程序属性
set_target_properties(QtEnterpriseApp PROPERTIES
    OUTPUT_NAME "QT Enterprise Example"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/../../Info.plist.in"
)