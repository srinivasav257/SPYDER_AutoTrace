# SPYDER AutoTrace

A "Lifetime-Stable" industry-grade CMake template for Qt 6 C++ projects.

## Architecture Highlights

This template is designed to be **Structural Complete**: you never change the file structure, only add new modules.

-   **100% Target-Based**: No global state. Uses `target_link_libraries`, `target_compile_options`.
-   **Modular**: All logic (Warnings, Sanitizers, Dependencies) is isolated in `cmake/` modules.
-   **Scalable**: Works for a single `main.cpp` or a 50-module enterprise app.
-   **Upgradeable**: Change `CMAKE_CXX_STANDARD` in `StandardSettings.cmake` to move to C++20/26.
-   **Testing**: Dual support for **GoogleTest** and **QtTest**.
-   **Cross-Platform**: Full support for Windows, Linux, and macOS.
-   **CI/CD Ready**: GitHub Actions workflow included.

## Structure

```
├── .github/
│   └── workflows/
│       └── ci.yml                  # GitHub Actions CI pipeline
├── cmake/
│   ├── AutoVersion.cmake           # Git-based version generation
│   ├── CodeFormatTargets.cmake     # clang-format & cmake-format targets
│   ├── CompilerWarnings.cmake      # Strict warnings (MSVC/Clang/GCC)
│   ├── Coverage.cmake              # Code coverage support (gcov/lcov)
│   ├── Deployment.cmake            # Platform-specific deployment
│   ├── Documentation.cmake         # Doxygen integration
│   ├── EnvConfig.cmake             # .env → C++ header generation
│   ├── FetchDependencies.cmake     # 3rd party libs with SHA256 verification
│   ├── InstallRules.cmake          # CMake install configuration
│   ├── PackageManager.cmake        # Conan/Vcpkg support
│   ├── Packaging.cmake             # CPack configuration
│   ├── PrecompiledHeaders.cmake    # PCH support
│   ├── PreventInSourceBuilds.cmake # Build hygiene
│   ├── QmlSupport.cmake            # Qt Quick/QML support
│   ├── Sanitizers.cmake            # ASan, TSan, UBSan, MSan
│   ├── StandardSettings.cmake      # C++ Standard, LTO, ccache
│   ├── StaticAnalyzers.cmake       # Clang-Tidy, CppCheck, IWYU
│   └── SuperBuild.cmake            # ExternalProject superbuild
├── src/
│   ├── CMakeLists.txt              # Main application target
│   └── ...
├── tests/
│   ├── CMakeLists.txt              # GTest & QtTest configurations
│   ├── tst_gtest_main.cpp
│   └── tst_qtest_main.cpp
├── scripts/
│   └── SignInstaller.ps1           # Windows code signing
├── CMakeLists.txt                  # Root orchestration
├── CMakePresets.json               # Modern CMake presets
├── Run_Build.bat                   # Windows build menu
└── run_build.sh                    # Linux/macOS build menu
```

## Usage

### Quick Start with CMake Presets (Recommended)

CMake Presets provide a modern, portable way to configure builds. Works on all platforms.

**List available presets:**
```bash
cmake --list-presets
```

**Configure and build:**
```bash
# Debug build with Ninja
cmake --preset debug
cmake --build --preset debug

# Release build with MSVC (Windows)
cmake --preset msvc-release
cmake --build --preset msvc-release
```

**Run tests:**
```bash
ctest --preset default
```

**Full workflow (configure → build → test):**
```bash
cmake --workflow --preset dev      # Development workflow
cmake --workflow --preset release  # Release workflow with packaging
cmake --workflow --preset ci       # CI workflow
```

### Available Presets

| Preset | Description |
|--------|-------------|
| `debug` | Debug build with Ninja |
| `release` | Release build with Ninja |
| `relwithdebinfo` | Release with debug info |
| `msvc-debug` | Debug with Visual Studio (Windows) |
| `msvc-release` | Release with Visual Studio (Windows) |
| `sanitizers` | Debug with Address + Leak sanitizers |
| `coverage` | Debug with code coverage enabled |
| `superbuild` | SuperBuild mode (deps + project) |
| `ci-linux` | CI configuration for Linux |
| `ci-macos` | CI configuration for macOS |
| `ci-windows` | CI configuration for Windows |

### Interactive Build Scripts

#### Windows
```batch
Run_Build.bat
```

#### Linux/macOS
```bash
chmod +x run_build.sh
./run_build.sh
```

Or run directly:
```bash
./run_build.sh debug      # Build debug
./run_build.sh release    # Build release
./run_build.sh test       # Run tests
./run_build.sh coverage   # Build with coverage
./run_build.sh package    # Create installer
```

### Manual CLI

**Build:**
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Windows (MSVC)
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Linux/macOS (Ninja)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Test:**
```bash
cd build
ctest --output-on-failure
```

## Code Coverage

Generate code coverage reports using gcov/lcov.

**Prerequisites (Linux/macOS):**
```bash
# Ubuntu/Debian
sudo apt install lcov

# macOS
brew install lcov

# Fedora/RHEL
sudo dnf install lcov
```

**Generate coverage report:**
```bash
# Using presets
cmake --preset coverage
cmake --build --preset coverage
cmake --build build/coverage --target coverage

# Manual
cmake -S . -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cmake --build build --target coverage
```

**View report:**
```bash
cmake --build build --target coverage-open
# Or open: build/coverage/html/index.html
```

## CI/CD with GitHub Actions

The template includes a complete GitHub Actions workflow (`.github/workflows/ci.yml`) that:

- Builds on **Windows**, **Linux**, and **macOS**
- Installs Qt automatically (cached for speed)
- Runs all tests with verbose output
- Checks code formatting (clang-format, cmake-format)
- Creates releases automatically when you push a tag

**Trigger a release:**
```bash
git tag v1.0.0
git push origin v1.0.0
```

## Dependency Management

Dependencies are fetched via `FetchContent` with **SHA256 verification** for security.

| Dependency | Version | Purpose |
|------------|---------|---------|
| GoogleTest | v1.14.0 | Unit testing |
| fmt | v10.1.1 | Modern C++ formatting |
| spdlog | v1.12.0 | Fast logging |

**Security:** SHA256 hashes protect against supply-chain attacks and ensure reproducible builds.

**Update a dependency:**
1. Change the URL/version in `cmake/FetchDependencies.cmake`
2. Download the archive and compute hash: `shasum -a 256 <file>`
3. Update the `URL_HASH` value

## Packaging & Deployment

This template supports one-click creation of **Installers (.exe)** and **Portable Zips**.

### Prerequisites
To generate the `.exe` installer, you need **NSIS**. Install it via Winget:
```powershell
winget install NSIS.NSIS
```

### Creating the Installer

**Using presets:**
```bash
cmake --preset msvc-release
cmake --build --preset msvc-release
cpack --preset windows-installer
```

**Using build script:**
1. Run `Run_Build.bat`
2. Select **Option 9: Create Installer (Package)**

The script will:
- Clean & Rebuild (Release)
- Run `windeployqt` to copy necessary Qt DLLs
- Run `CPack` to generate the installer
- **Auto-Sign** the installer using a generated Self-Signed Certificate

**Output Location:** `build/QtTemplateProject-*-win64.exe` (and `.zip`)

### Package Presets

| Preset | Platforms | Generators |
|--------|-----------|------------|
| `default` | All | ZIP |
| `windows-installer` | Windows | NSIS, ZIP |
| `linux-packages` | Linux | DEB, RPM, TGZ |
| `macos-bundle` | macOS | DragNDrop, ZIP |

### Code Signing
The project includes a helper script (`scripts/SignInstaller.ps1`) that:
- Automatically looks for `signtool.exe` (Windows SDK).
- Generates a **Self-Signed Certificate** (`QtTemplateDevCert.pfx`) if one is missing.
- Signs both the Application Binary (`QtTemplateApp.exe`) and the Installer (`setup.exe`).

> **Note:** Because the certificate is self-signed, Windows Defender/SmartScreen will show an "Unknown Publisher" warning. To remove this, replace the generated `.pfx` with a valid Code Signing Certificate bought from a generic CA.

## Configuration (.env)

The project supports dynamic environment configuration.
1. Create a `.env` file in the root (copy from `.env.example`)
2. Define variables like `API_URL=https://api.staging.com`
3. These are compiled into `src/generated/env_config.h`

**Example `.env`:**
```
API_URL=https://api.production.com
API_TIMEOUT_MS=10000
ENABLE_DEBUG_MENU=false
```

## Advanced Features

### Sanitizers (ASan/TSan/UBSan)

**Using presets:**
```bash
cmake --preset sanitizers
cmake --build --preset sanitizers
```

**Manual:**
```bash
cmake -S . -B build -DENABLE_SANITIZER_ADDRESS=ON -DENABLE_SANITIZER_LEAK=ON
```

Available sanitizers:
- `ENABLE_SANITIZER_ADDRESS` - Address Sanitizer (memory errors)
- `ENABLE_SANITIZER_LEAK` - Leak Sanitizer (memory leaks)
- `ENABLE_SANITIZER_THREAD` - Thread Sanitizer (data races)
- `ENABLE_SANITIZER_UNDEFINED` - Undefined Behavior Sanitizer
- `ENABLE_SANITIZER_MEMORY` - Memory Sanitizer (uninitialized reads)

### Static Analysis

```bash
cmake -S . -B build -DENABLE_CLANG_TIDY=ON -DENABLE_CPPCHECK=ON
```

### Link-Time Optimization (LTO)

```bash
cmake -S . -B build -DENABLE_IPO=ON -DCMAKE_BUILD_TYPE=Release
```

### Precompiled Headers

Enabled by default for faster compilation. Disable with:
```bash
cmake -S . -B build -DENABLE_PCH=OFF
```

---

## TestExecutor Module - Professional No-Code Test Automation

The **TestExecutor** module provides a complete, professional-grade test automation framework for Automotive Infotainment ECU testing. It enables **no-code test creation** where users select predefined commands and configure parameters without writing code.

### Key Features

- **No-Code Test Design**: Create tests by selecting commands and filling parameters
- **JSON-Based Storage**: Tests stored in structured JSON with mandatory fields
- **Multi-Protocol Support**: Serial/UART, CAN (Vector interfaces), Power Supply control
- **Real-Time Monitoring**: Live execution progress with detailed logging
- **Professional Reports**: HTML reports similar to pytest-html, plus JSON/CSV/XML exports

### Architecture

```
src/TestExecutor/
├── include/
│   ├── TestDataModels.h         # TestCase, TestStep, TestResult structs
│   ├── TestRepository.h         # Tree model and JSON persistence
│   ├── CommandRegistry.h        # Predefined command definitions
│   ├── TestExecutorEngine.h     # Execution engine with threading
│   ├── TestExplorerPanel.h      # Test browsing/management UI
│   ├── TestEditorPanel.h        # Test editing UI with step config
│   ├── TestProgressPanel.h      # Execution monitoring UI
│   ├── TestReportGenerator.h    # Multi-format report generation
│   └── TestExecutorPanels.h     # Panel registration
└── src/
    └── [implementations]
```

### UI Panels

| Panel | Purpose |
|-------|---------|
| **Test Explorer** | Browse, import/export, search, and manage test cases |
| **Test Editor** | Edit test metadata and configure step parameters |
| **Test Progress** | Real-time execution monitoring with results table and log |

### Predefined Commands

#### Serial/UART Commands
| Command | Description |
|---------|-------------|
| `serial_enter_md_session` | Enter Manufacturing Diagnostics mode |
| `serial_exit_md_session` | Exit Manufacturing Diagnostics mode |
| `serial_send_mandiag` | Send ManDiag hex command |
| `serial_read_did` | Read Data Identifier (DID) |
| `serial_write_did` | Write Data Identifier (DID) |
| `serial_send_raw` | Send raw serial data |
| `serial_read_response` | Read serial response with timeout |

#### CAN Commands
| Command | Description |
|---------|-------------|
| `can_send_message` | Send CAN message (Vector interface) |
| `can_read_message` | Read CAN message with filter |
| `can_check_signal` | Check CAN signal value |

#### Power Supply Commands
| Command | Description |
|---------|-------------|
| `power_turn_on` / `power_turn_off` | Control power output |
| `power_set_voltage` | Set output voltage |
| `power_set_current_limit` | Set current limit |
| `power_read_measurements` | Read voltage/current measurements |

#### Flow Control Commands
| Command | Description |
|---------|-------------|
| `wait` | Wait for specified duration |
| `repeat` | Repeat steps N times |
| `log_message` | Log custom message |
| `set_variable` | Set runtime variable |

#### Validation Commands
| Command | Description |
|---------|-------------|
| `assert_equals` | Assert exact value match |
| `assert_contains` | Assert substring presence |
| `assert_regex` | Assert regex pattern match |
| `assert_range` | Assert value within range |

### Test Case JSON Format

Tests are stored in JSON with mandatory metadata fields:

```json
{
    "testCases": [
        {
            "id": "tc_001_enter_md_session",
            "name": "Enter Manufacturing Diagnostics Session",
            "description": "Verify system can enter MD mode via UART",
            "requirementId": "REQ-DIAG-001",
            "requirementLink": "https://confluence.company.com/REQ-DIAG-001",
            "jiraTicket": "INFO-1234",
            "priority": "High",
            "tags": ["md_session", "uart"],
            "steps": [
                {
                    "id": "step_1",
                    "order": 1,
                    "category": "Serial",
                    "command": "serial_enter_md_session",
                    "description": "Connect and enter MD session",
                    "parameters": {
                        "port": "COM3",
                        "baud_rate": "115200",
                        "timeout": "5000"
                    }
                },
                {
                    "id": "step_2",
                    "order": 2,
                    "category": "Validation",
                    "command": "assert_contains",
                    "description": "Verify MD session prompt",
                    "parameters": {
                        "expected": "MD>",
                        "case_sensitive": "false"
                    }
                }
            ]
        }
    ]
}
```

### Report Generation

Export test results in multiple formats:

| Format | Description |
|--------|-------------|
| **HTML** | Professional styled report with summary cards, progress bar, and results table |
| **JSON** | Machine-readable with full session/step data |
| **CSV** | Spreadsheet-compatible for Excel/Sheets |
| **XML** | JUnit-compatible for CI/CD integration |

### Usage

**1. Register panels at application startup:**
```cpp
#include "TestExecutorPanels.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Register TestExecutor panels before creating MainWindow
    TestExecutor::registerTestExecutorPanels();
    
    DockManager::DockMainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
```

**2. Import tests from JSON:**
- Open Test Explorer panel
- Click "Import" button
- Select JSON test file (see `resources/sample_tests.json`)

**3. Edit tests:**
- Double-click a test in Explorer to open in Editor
- Configure mandatory fields (Name, Description, Requirement, JIRA)
- Add/edit steps by selecting category → command → parameters

**4. Execute tests:**
- Select tests in Explorer
- Click "Run" in Progress panel
- Monitor real-time execution and view results

**5. Export reports:**
- After execution, click "Export Report" in Progress panel
- Choose format (HTML recommended)

### Sample Tests

A sample test file is provided at `resources/sample_tests.json` with 5 example test cases:

1. **Enter MD Session** - Basic UART connectivity test
2. **Read ECU Part Number** - DID read operation
3. **CAN Bus Communication** - CAN message exchange
4. **Power Cycle Test** - Power supply control sequence
5. **Write and Verify VIN** - DID write with verification (uses variables)

---

## Requirements

- **CMake**: 3.21+ (for presets) or 3.20+ (without presets)
- **Qt**: 6.2+
- **Compiler**: MSVC 2019+, GCC 10+, Clang 12+
- **Optional**: Ninja, lcov, clang-format, clang-tidy

## License

This template is provided as-is for use in your projects.
