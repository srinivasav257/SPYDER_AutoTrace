# DockManager library
add_subdirectory(DockManager)

# Application source files - panels only, DockManager provides the framework
set(PROJECT_SOURCES
    main.cpp
    panels/SamplePanels.cpp
    panels/SamplePanels.h
    panels/TestTreeModel.cpp
    panels/TestTreeModel.h
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
    Qt6::Widgets
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
