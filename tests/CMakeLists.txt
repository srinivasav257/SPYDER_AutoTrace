
# ------------------------------------------------------------------------------
# Testing Configuration
# ------------------------------------------------------------------------------

# Enable CTest integration
enable_testing()

# Output test binaries to a separate directory so they don't end up
# alongside the application binary or get picked up by CPack
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

# 1. GoogleTest Suite (trivial placeholder tests)
# -------------------
add_executable(UnitTests_GTest tst_gtest_main.cpp)
target_link_libraries(UnitTests_GTest PRIVATE
    GTest::gtest
    Qt6::Core
)

# Discover tests specifically for this target
include(GoogleTest)
# Avoid running the test executable at build time (can fail if Qt DLLs
# aren't on PATH yet). Discover at test time instead.
gtest_discover_tests(UnitTests_GTest DISCOVERY_MODE PRE_TEST)

# 2. Qt Test Suite
# ----------------
add_executable(UnitTests_QtTest tst_qtest_main.cpp)
target_include_directories(UnitTests_QtTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels"
)
target_sources(UnitTests_QtTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels/HWConfigManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels/SerialConfigWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels/CANConfigWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels/HWConfigDialog.cpp"
)
target_link_libraries(UnitTests_QtTest PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
    Qt6::SerialPort
    SerialManager::SerialManager
    CANManager::CANManager
    DBCManager::DBCManager
    StyleLib::StyleLib
)

# Register with CTest
add_test(NAME QtTestCommand COMMAND UnitTests_QtTest)

# ==============================================================================
# 3. TestDataModels JSON round-trip tests
# ==============================================================================
add_executable(UnitTests_TestDataModels tst_TestDataModels.cpp)
target_link_libraries(UnitTests_TestDataModels PRIVATE
    GTest::gtest_main
    TestExecutor::TestExecutor
    Qt6::Core
    Qt6::Widgets
)
gtest_discover_tests(UnitTests_TestDataModels DISCOVERY_MODE PRE_TEST)

# ==============================================================================
# 4. CommandRegistry tests (stub execution, validation, lookup)
# ==============================================================================
add_executable(UnitTests_CommandRegistry tst_CommandRegistry.cpp)
target_include_directories(UnitTests_CommandRegistry PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels"
)
target_link_libraries(UnitTests_CommandRegistry PRIVATE
    GTest::gtest_main
    TestExecutor::TestExecutor
    ManDiag::ManDiag
    SerialManager::SerialManager
    CANManager::CANManager
    DBCManager::DBCManager
    Qt6::Core
    Qt6::Widgets
    Qt6::SerialPort
)
# HWConfigManager.cpp is not in a library, so compile it directly
target_sources(UnitTests_CommandRegistry PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels/HWConfigManager.cpp"
)
gtest_discover_tests(UnitTests_CommandRegistry DISCOVERY_MODE PRE_TEST)

# ==============================================================================
# 5. DBCParser tests (parsing, encoding, decoding, index)
# ==============================================================================
add_executable(UnitTests_DBCParser tst_DBCParser.cpp)
target_link_libraries(UnitTests_DBCParser PRIVATE
    GTest::gtest_main
    DBCManager::DBCManager
    Qt6::Core
)
gtest_discover_tests(UnitTests_DBCParser DISCOVERY_MODE PRE_TEST)

# ==============================================================================
# 6. HWConfigManager save/load round-trip tests
# ==============================================================================
add_executable(UnitTests_HWConfigManager tst_HWConfigManager.cpp)
target_include_directories(UnitTests_HWConfigManager PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels"
)
target_link_libraries(UnitTests_HWConfigManager PRIVATE
    GTest::gtest_main
    SerialManager::SerialManager
    Qt6::Core
    Qt6::SerialPort
)
# HWConfigManager.cpp is not in a library, so compile it directly
target_sources(UnitTests_HWConfigManager PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/panels/HWConfigManager.cpp"
)
gtest_discover_tests(UnitTests_HWConfigManager DISCOVERY_MODE PRE_TEST)
