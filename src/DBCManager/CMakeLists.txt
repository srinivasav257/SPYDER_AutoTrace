# DBCManager Module — DBC File Parser & CAN Database Manager
add_library(DBCManager STATIC
    src/DBCParser.cpp
    src/DBCManager.cpp
    include/DBCParser.h
    include/DBCManager.h
)

add_library(DBCManager::DBCManager ALIAS DBCManager)

target_include_directories(DBCManager
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(DBCManager
    PUBLIC
        Qt6::Core
)

set_target_properties(DBCManager PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
