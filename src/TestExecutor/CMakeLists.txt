# TestExecutor Module - Professional Test Automation Framework
# Provides:
#   - Test data models (TestCase, TestStep, TestResult)
#   - JSON-based test repository
#   - Test execution engine
#   - UI panels (Explorer, Editor, Progress)
#   - HTML report generation

add_library(TestExecutor STATIC
    # Data Models
    src/TestDataModels.cpp
    src/TestRepository.cpp
    src/CommandRegistry.cpp
    
    # Execution Engine
    src/TestExecutorEngine.cpp
    
    # UI Panels
    src/TestExplorerPanel.cpp
    src/TestEditorPanel.cpp
    src/TestProgressPanel.cpp
    
    # Panel Registration
    src/TestExecutorPanels.cpp
    
    # Report Generation
    src/TestReportGenerator.cpp
    
    # Headers (for IDE integration)
    include/TestDataModels.h
    include/TestRepository.h
    include/CommandRegistry.h
    include/TestExecutorEngine.h
    include/TestExplorerPanel.h
    include/TestEditorPanel.h
    include/TestProgressPanel.h
    include/TestReportGenerator.h
    include/TestExecutorPanels.h
)

add_library(TestExecutor::TestExecutor ALIAS TestExecutor)

target_include_directories(TestExecutor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(TestExecutor
    PUBLIC
        Qt6::Widgets
        Qt6::Core
    PRIVATE
        DockManager::DockManager
        SerialManager::SerialManager
        CANManager::CANManager
)

# Allow TestExecutor to access HWConfigManager for port alias resolution
target_include_directories(TestExecutor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../panels"
)

# Enable automoc for Q_OBJECT macros
set_target_properties(TestExecutor PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
