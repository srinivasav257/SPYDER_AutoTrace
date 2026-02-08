# StyleLib - Centralized theme and stylesheet management

set(STYLELIB_HEADERS
    include/ThemeCatalog.h
    include/ThemeManager.h
    include/PerformanceProxyStyle.h
)

set(STYLELIB_SOURCES
    src/ThemeCatalog.cpp
    src/ThemeManager.cpp
    src/PerformanceProxyStyle.cpp
)

add_library(StyleLib STATIC ${STYLELIB_HEADERS} ${STYLELIB_SOURCES})

target_include_directories(StyleLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(StyleLib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(StyleLib PUBLIC cxx_std_17)

add_library(StyleLib::StyleLib ALIAS StyleLib)

set_target_properties(StyleLib PROPERTIES FOLDER "Libraries")
