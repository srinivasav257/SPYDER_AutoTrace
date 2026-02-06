# SerialManager Module - Serial Port Communication Framework
# Provides:
#   - Centralized serial port management (singleton pattern)
#   - Thread-safe port access
#   - Configuration management
#   - Send/receive with timeout support
#   - Response matching functionality

add_library(SerialManager STATIC
    src/SerialManager.cpp
    
    # Headers (for IDE integration)
    include/SerialManager.h
)

add_library(SerialManager::SerialManager ALIAS SerialManager)

target_include_directories(SerialManager
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(SerialManager
    PUBLIC
        Qt6::Core
        Qt6::SerialPort
)

# Enable automoc for Q_OBJECT macros
set_target_properties(SerialManager PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
