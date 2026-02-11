# DockManager library
add_subdirectory(DockManager)

# SerialManager library - serial port communication framework
add_subdirectory(SerialManager)

# CANManager library - CAN bus communication framework (Vector XL, Kvaser)
add_subdirectory(CANManager)

# DBCManager library - DBC file parser, CAN database, encode/decode
add_subdirectory(DBCManager)

# TestExecutor library - professional test automation framework
add_subdirectory(TestExecutor)

# ManDiag library - Manufacturing Diagnostics framework (EOL/MOL commands)
add_subdirectory(ManDiag)

# Application source files - panels only, DockManager provides the framework
set(PROJECT_SOURCES
    main.cpp
    panels/SamplePanels.cpp
    panels/SamplePanels.h
    panels/HWConfigManager.h
    panels/HWConfigManager.cpp
    panels/SerialConfigWidget.h
    panels/SerialConfigWidget.cpp
    panels/CANConfigWidget.h
    panels/CANConfigWidget.cpp
    panels/HWConfigDialog.h
    panels/HWConfigDialog.cpp
    "${CMAKE_SOURCE_DIR}/resources/resources.qrc"
)

# Create the executable target
add_executable(SPYDER_AutoTrace ${PROJECT_SOURCES})

# Include directories for application-specific panels
target_include_directories(SPYDER_AutoTrace PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/panels"
)

# Link DockManager - it brings in qtads and Qt dependencies automatically
target_link_libraries(SPYDER_AutoTrace PRIVATE
    DockManager::DockManager
    TestExecutor::TestExecutor
    ManDiag::ManDiag
    SerialManager::SerialManager
    CANManager::CANManager
    DBCManager::DBCManager
    Qt6::Widgets
    Qt6::SerialPort
)

# Apply settings via Interface Targets
set_project_warnings(SPYDER_AutoTrace)
enable_sanitizers(SPYDER_AutoTrace)
target_include_version(SPYDER_AutoTrace)

# Precompiled Headers (Speed up compilation)
enable_pch(SPYDER_AutoTrace HEADERS
    <QApplication>
    <QMainWindow>
    <QLabel>
    <QString>
    <vector>
    <memory>
)

# Platform specific settings
if(WIN32)
    set_target_properties(SPYDER_AutoTrace PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Configure Installation & Exports (must come BEFORE deployment for correct install order)
# This generates QtTemplateProjectConfig.cmake so others can find_package() it
setup_install_rules(SPYDER_AutoTrace SPYDER_AutoTrace)

# Enable deployment targets (make QtTemplateApp_deploy)
# Note: Must be after setup_install_rules() so windeployqt runs after exe is installed
add_deployment_support(SPYDER_AutoTrace)

# Generate typed Environment Header (env_config.h)
generate_env_header(SPYDER_AutoTrace)
