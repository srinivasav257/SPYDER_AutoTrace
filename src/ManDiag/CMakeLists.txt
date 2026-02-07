# ManDiag Module - Manufacturing Diagnostics Framework
# Provides:
#   - ManDiag protocol parsing and utilities
#   - EOL (End of Line) commands - Serial interface
#   - MOL (Mid of Line) commands - CAN interface (placeholder)
#   - Integration with CommandRegistry for GUI test automation
#
# Command Types:
#   - MD_Tx: Send command only (fire and forget)
#   - MD_TxRx: Send and match entire response
#   - MD_Type1: Send, parse, and validate individual fields

add_library(ManDiag STATIC
    # Protocol layer
    src/ManDiagProtocol.cpp
    
    # EOL Commands (Serial)
    src/ManDiagEOL.cpp
    
    # MOL Commands (CAN - placeholder)
    src/ManDiagMOL.cpp
    
    # Headers (for IDE integration)
    include/ManDiagProtocol.h
    include/ManDiagEOL.h
    include/ManDiagMOL.h
)

add_library(ManDiag::ManDiag ALIAS ManDiag)

target_include_directories(ManDiag
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Access to TestExecutor for CommandRegistry integration
        ${CMAKE_CURRENT_SOURCE_DIR}/../TestExecutor/include
    PUBLIC
        # Shared utilities (HexUtils.h) - PUBLIC because ManDiagProtocol.h includes it
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
