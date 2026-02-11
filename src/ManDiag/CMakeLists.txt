# ManDiag Module - Manufacturing Diagnostics Framework
# Protocols:
#   - ITS  : Implemented (Serial) with fixed/variable response commands
#   - PITS : Placeholder scaffold
#   - MOL  : Placeholder scaffold
# Structure:
#   - Shared protocol parsing utilities in core/
#   - Protocol-specific handlers in protocols/

add_library(ManDiag STATIC
    # Core protocol parsing
    src/core/ManDiagProtocol.cpp

    # Protocol implementations
    src/protocols/ManDiagITS/ManDiagITS.cpp
    src/protocols/ManDiagPITS/ManDiagPITS.cpp
    src/protocols/ManDiagMOL/ManDiagMOL.cpp

    # Public headers
    include/ManDiag.h
    include/core/ManDiagProtocol.h
    include/protocols/ManDiagITS/ManDiagITS.h
    include/protocols/ManDiagPITS/ManDiagPITS.h
    include/protocols/ManDiagMOL/ManDiagMOL.h
)

add_library(ManDiag::ManDiag ALIAS ManDiag)

target_include_directories(ManDiag
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Command definitions and category enums
        ${CMAKE_CURRENT_SOURCE_DIR}/../TestExecutor/include
    PUBLIC
        # Shared hex utilities included by public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
)

target_link_libraries(ManDiag
    PUBLIC
        Qt6::Core
    PRIVATE
        SerialManager::SerialManager
        TestExecutor::TestExecutor
)

# Enable automoc for Q_OBJECT macros
set_target_properties(ManDiag PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Precompiled headers for faster compilation
target_precompile_headers(ManDiag PRIVATE
    <QString>
    <QByteArray>
    <QVariant>
    <QDebug>
)
