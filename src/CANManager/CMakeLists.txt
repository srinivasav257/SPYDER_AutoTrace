# CANManager Module — CAN Bus Communication Framework
# Provides:
#   - Abstract CAN driver interface (ICANDriver)
#   - Vector XL Library driver (runtime DLL loading, CAN HS + FD)
#   - Centralized CAN bus manager (singleton, multi-channel slots)
#   - Future: Kvaser, SocketCAN driver backends

add_library(CANManager STATIC
    src/VectorCANDriver.cpp
    src/CANManager.cpp

    # Headers (for IDE integration / AUTOMOC)
    include/CANInterface.h
    include/VectorCANDriver.h
    include/CANManager.h
)

add_library(CANManager::CANManager ALIAS CANManager)

target_include_directories(CANManager
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        # Vector XL Library headers (vxlapi.h) — PUBLIC because
        # CANManager.h includes VectorCANDriver.h which includes vxlapi.h
        "${CMAKE_SOURCE_DIR}/third_party/vector_xl/include"
)

target_link_libraries(CANManager
    PUBLIC
        Qt6::Core
)

# Enable automoc for Q_OBJECT macros
set_target_properties(CANManager PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# The Vector XL Library DLL is loaded at runtime via QLibrary,
# so we do NOT link against vxlapi.lib here. This means the
# application compiles and runs even without Vector drivers installed.
