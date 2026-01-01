# ECharts模块的CMakeLists.txt

qt_add_library(ECharts SHARED
    echartstab.cpp
    echartstab.h
)

target_link_libraries(ECharts PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    Analytics
)

# 设置头文件导出
set_target_properties(ECharts PROPERTIES
    PUBLIC_HEADER echartstab.h
)

# 安装规则
install(TARGETS ECharts
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/echarts
)