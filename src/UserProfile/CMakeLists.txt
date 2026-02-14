qt6_add_library(userprofile STATIC
    userinfopage.h
    userinfopage.cpp
    userprofiletab.h
    userprofiletab.cpp
)

if(Qt6Charts_FOUND)
    target_link_libraries(userprofile PUBLIC Qt6::Charts)
    target_compile_definitions(userprofile PRIVATE HAS_QT_CHARTS)
endif()

if(Qt6Pdf_FOUND)
    target_link_libraries(userprofile PRIVATE Qt6::Pdf)
    target_compile_definitions(userprofile PRIVATE HAS_QT_PDF)
endif()

target_link_libraries(userprofile PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    EnterpriseCore
    EnterpriseServices
    network
    auth
    Styles
)

target_include_directories(userprofile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Analytics
    ${CMAKE_SOURCE_DIR}/src/App
)