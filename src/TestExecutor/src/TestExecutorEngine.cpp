/**
 * @file TestExecutorEngine.cpp
 * @brief Implementation of the Test Execution Engine.
 */

#include "TestExecutorEngine.h"
#include "TestRepository.h"
#include "CommandRegistry.h"
#include <SerialManager.h>
#include "HWConfigManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

using namespace SerialManager;

namespace TestExecutor {

//=============================================================================
// TestConfiguration Implementation
//=============================================================================

QVariantMap TestConfiguration::toVariantMap() const
{
    QVariantMap map;
    map["serialPort"] = serialPort;
    map["serialBaudRate"] = serialBaudRate;
    map["serialDataBits"] = serialDataBits;
    map["serialStopBits"] = serialStopBits;
    map["serialParity"] = serialParity;
    map["serialTimeoutMs"] = serialTimeoutMs;
    
    map["canInterface"] = canInterface;
    map["canChannel"] = canChannel;
    map["canBitrate"] = canBitrate;
    map["canDataBitrate"] = canDataBitrate;
    map["canFdEnabled"] = canFdEnabled;
    
    map["powerSupplyType"] = powerSupplyType;
    map["powerSupplyPort"] = powerSupplyPort;
    map["defaultVoltage"] = defaultVoltage;
    map["maxCurrent"] = maxCurrent;
    
    map["stopOnFirstFailure"] = stopOnFirstFailure;
    map["continueOnError"] = continueOnError;
    map["stepDelayMs"] = stepDelayMs;
    map["defaultTimeoutMs"] = defaultTimeoutMs;
    
    map["logLevel"] = logLevel;
    map["logFilePath"] = logFilePath;
    map["logToFile"] = logToFile;
    
    map["reportOutputPath"] = reportOutputPath;
    map["reportTemplate"] = reportTemplate;
    map["autoGenerateReport"] = autoGenerateReport;
    
    return map;
}

TestConfiguration TestConfiguration::fromVariantMap(const QVariantMap& map)
{
    TestConfiguration config;
    
    config.serialPort = map.value("serialPort", "COM3").toString();
    config.serialBaudRate = map.value("serialBaudRate", 115200).toInt();
    config.serialDataBits = map.value("serialDataBits", 8).toInt();
    config.serialStopBits = map.value("serialStopBits", 1).toInt();
    config.serialParity = map.value("serialParity", "None").toString();
    config.serialTimeoutMs = map.value("serialTimeoutMs", 1000).toInt();
    
    config.canInterface = map.value("canInterface", "Vector").toString();
    config.canChannel = map.value("canChannel", 0).toInt();
    config.canBitrate = map.value("canBitrate", 500000).toInt();
    config.canDataBitrate = map.value("canDataBitrate", 2000000).toInt();
    config.canFdEnabled = map.value("canFdEnabled", true).toBool();
    
    config.powerSupplyType = map.value("powerSupplyType", "Keysight").toString();
    config.powerSupplyPort = map.value("powerSupplyPort").toString();
    config.defaultVoltage = map.value("defaultVoltage", 12.0).toDouble();
    config.maxCurrent = map.value("maxCurrent", 10.0).toDouble();
    
    config.stopOnFirstFailure = map.value("stopOnFirstFailure", false).toBool();
    config.continueOnError = map.value("continueOnError", false).toBool();
    config.stepDelayMs = map.value("stepDelayMs", 100).toInt();
    config.defaultTimeoutMs = map.value("defaultTimeoutMs", 5000).toInt();
    
    config.logLevel = map.value("logLevel", "INFO").toString();
    config.logFilePath = map.value("logFilePath").toString();
    config.logToFile = map.value("logToFile", true).toBool();
    
    config.reportOutputPath = map.value("reportOutputPath").toString();
    config.reportTemplate = map.value("reportTemplate", "default").toString();
    config.autoGenerateReport = map.value("autoGenerateReport", true).toBool();
    
    return config;
}

QJsonObject TestConfiguration::toJson() const
{
    return QJsonObject::fromVariantMap(toVariantMap());
}

TestConfiguration TestConfiguration::fromJson(const QJsonObject& json)
{
    return fromVariantMap(json.toVariantMap());
}

//=============================================================================
// TestExecutorEngine Implementation
//=============================================================================

TestExecutorEngine& TestExecutorEngine::instance()
{
    static TestExecutorEngine instance;
    return instance;
}

TestExecutorEngine::TestExecutorEngine()
{
    // Initialize command registry
    CommandRegistry::instance().registerBuiltinCommands();
}

TestExecutorEngine::~TestExecutorEngine()
{
    if (m_workerThread && m_workerThread->isRunning()) {
        m_stopRequested = true;
        m_pauseCondition.wakeAll();
        m_workerThread->quit();
        m_workerThread->wait(5000);
    }
}

void TestExecutorEngine::setConfiguration(const TestConfiguration& config)
{
    QMutexLocker locker(&m_mutex);
    m_config = config;
}

bool TestExecutorEngine::loadConfiguration(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        return false;
    }
    
    m_config = TestConfiguration::fromJson(doc.object());
    return true;
}

bool TestExecutorEngine::saveConfiguration(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument doc(m_config.toJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

void TestExecutorEngine::runTests(const QStringList& testCaseIds)
{
    if (m_state != ExecutorState::Idle) {
        qWarning() << "Engine is already running";
        return;
    }
    
    m_pendingTestIds = testCaseIds;
    m_stopRequested = false;
    m_pauseRequested = false;
    
    // Sync serial/CAN settings from HWConfigManager (single source of truth)
    syncFromHWConfig();
    
    // Create new session
    m_currentSession = std::make_unique<TestSession>();
    m_currentSession->id = TestSession::generateId();
    m_currentSession->name = QString("Test Run %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    m_currentSession->totalTests = testCaseIds.size();
    m_currentSession->configuration = m_config.toVariantMap();
    m_currentSession->startTime = QDateTime::currentDateTime();
    
    setState(ExecutorState::Running);
    emit sessionStarted(m_currentSession->id, testCaseIds.size());
    
    // Clean up previous worker thread if any
    if (m_workerThread) {
        if (m_workerThread->isRunning()) {
            m_workerThread->quit();
            m_workerThread->wait();
        }
        delete m_workerThread;
        m_workerThread = nullptr;
    }

    // Execute tests in a worker thread
    m_workerThread = QThread::create([this]() { executeTests(); });
    m_workerThread->start();
}

void TestExecutorEngine::runAllTests()
{
    QStringList allIds;
    for (const auto& tc : TestRepository::instance().allTestCases()) {
        if (tc.enabled) {
            allIds.append(tc.id);
        }
    }
    runTests(allIds);
}

void TestExecutorEngine::runSingleTest(const QString& testCaseId)
{
    runTests({testCaseId});
}

void TestExecutorEngine::runSingleStep(const QString& testCaseId, int stepIndex)
{
    // Get the test case
    const TestCase* tc = TestRepository::instance().testCase(testCaseId);
    if (!tc || stepIndex < 0 || stepIndex >= tc->steps.size()) {
        emit logMessage("ERROR", "Invalid test case or step index");
        return;
    }
    
    // Execute just this step
    TestStep step = tc->steps[stepIndex];
    emit stepStarted(testCaseId, stepIndex, step.description);
    
    m_stepTimer.start();
    TestStep result = executeStep(step, stepIndex, testCaseId);
    
    emit stepCompleted(testCaseId, stepIndex, result);
}

void TestExecutorEngine::pause()
{
    if (m_state == ExecutorState::Running) {
        m_pauseRequested = true;
        setState(ExecutorState::Paused);
    }
}

void TestExecutorEngine::resume()
{
    if (m_state == ExecutorState::Paused) {
        m_pauseRequested = false;
        m_pauseCondition.wakeAll();
        setState(ExecutorState::Running);
    }
}

void TestExecutorEngine::stop()
{
    if (m_state == ExecutorState::Running || m_state == ExecutorState::Paused) {
        setState(ExecutorState::Stopping);
        m_stopRequested = true;
        m_pauseCondition.wakeAll(); // Wake if paused
    }
}

void TestExecutorEngine::emergencyStop()
{
    m_stopRequested = true;
    m_pauseCondition.wakeAll();
    
    if (m_workerThread && m_workerThread->isRunning()) {
        // Cooperative cancellation: request interruption first, then wait
        m_workerThread->requestInterruption();
        if (!m_workerThread->wait(3000)) {
            // Thread did not finish within timeout — log but do NOT call terminate()
            emit logMessage("ERROR", "Worker thread did not stop within 3 s after interruption request");
        }
    }
    
    setState(ExecutorState::Idle);
    emit logMessage("WARNING", "Emergency stop executed");
}

void TestExecutorEngine::setState(ExecutorState state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(state);
    }
}

void TestExecutorEngine::executeTests()
{
    m_sessionTimer.start();
    auto& repo = TestRepository::instance();
    
    int testIndex = 0;
    for (const QString& testId : m_pendingTestIds) {
        // Check for stop request
        if (m_stopRequested) {
            emit logMessage("INFO", "Test execution stopped by user");
            break;
        }
        
        // Handle pause
        if (m_pauseRequested) {
            emit logMessage("INFO", "Test execution paused");
            QMutexLocker locker(&m_mutex);
            m_pauseCondition.wait(&m_mutex);
            if (m_stopRequested) break;
            emit logMessage("INFO", "Test execution resumed");
        }
        
        // Get test case
        const TestCase* tc = repo.testCase(testId);
        if (!tc) {
            emit logMessage("WARNING", "Test case not found: " + testId);
            continue;
        }
        
        // Skip disabled tests
        if (!tc->enabled) {
            TestResult result;
            result.testCaseId = testId;
            result.testCaseName = tc->name;
            result.status = TestStatus::Skipped;
            result.statusMessage = "Test is disabled";
            m_currentSession->skippedTests++;
            m_currentSession->results.append(result);
            emit testCompleted(result);
            continue;
        }
        
        emit testStarted(testId, tc->name, testIndex, m_pendingTestIds.size());
        emit logMessage("INFO", QString("Starting test: %1").arg(tc->name));
        
        // Execute test case
        m_testTimer.start();
        TestResult result = executeTestCase(*tc);
        
        // Update session statistics
        m_currentSession->results.append(result);
        switch (result.status) {
            case TestStatus::Passed:
                m_currentSession->passedTests++;
                break;
            case TestStatus::Failed:
                m_currentSession->failedTests++;
                break;
            case TestStatus::Error:
                m_currentSession->errorTests++;
                break;
            case TestStatus::Skipped:
                m_currentSession->skippedTests++;
                break;
            default:
                break;
        }
        
        emit testCompleted(result);
        emit logMessage("INFO", QString("Test %1: %2 (%3 ms)")
                       .arg(tc->name)
                       .arg(TestResult::statusToString(result.status))
                       .arg(result.durationMs));
        
        // Check for stop on first failure
        if (m_config.stopOnFirstFailure && 
            (result.status == TestStatus::Failed || result.status == TestStatus::Error)) {
            emit logMessage("INFO", "Stopping on first failure");
            break;
        }
        
        testIndex++;
        
        // Add delay between tests
        if (m_config.stepDelayMs > 0) {
            QThread::msleep(m_config.stepDelayMs);
        }
    }
    
    // Finalize session
    m_currentSession->endTime = QDateTime::currentDateTime();
    m_currentSession->durationMs = m_sessionTimer.elapsed();
    
    emit sessionCompleted(*m_currentSession);
    emit logMessage("INFO", QString("Session completed: %1 passed, %2 failed, %3 skipped")
                   .arg(m_currentSession->passedTests)
                   .arg(m_currentSession->failedTests)
                   .arg(m_currentSession->skippedTests));
    
    setState(ExecutorState::Idle);
}

TestResult TestExecutorEngine::executeTestCase(const TestCase& testCase)
{
    TestResult result;
    result.testCaseId = testCase.id;
    result.testCaseName = testCase.name;
    result.requirementId = testCase.requirementId;
    result.requirementLink = testCase.requirementLink;
    result.jiraTicket = testCase.jiraTicket;
    result.jiraLink = testCase.jiraLink;
    result.totalSteps = testCase.enabledStepCount();
    result.startTime = QDateTime::currentDateTime();
    
    bool overallSuccess = true;
    int stepIndex = 0;
    
    for (const TestStep& step : testCase.steps) {
        // Check for stop request
        if (m_stopRequested) {
            result.status = TestStatus::Skipped;
            result.statusMessage = "Stopped by user";
            break;
        }
        
        // Skip disabled steps
        if (!step.enabled) {
            result.skippedSteps++;
            continue;
        }
        
        emit stepStarted(testCase.id, stepIndex, step.description);
        emit testProgress(testCase.id, stepIndex, testCase.enabledStepCount());
        
        // Execute the step
        m_stepTimer.start();
        TestStep stepResult = executeStep(step, stepIndex, testCase.id);
        
        result.stepResults.append(stepResult);
        
        if (stepResult.status == TestStatus::Passed) {
            result.passedSteps++;
        } else if (stepResult.status == TestStatus::Failed || stepResult.status == TestStatus::Error) {
            result.failedSteps++;
            overallSuccess = false;
            
            if (!step.continueOnFail && !m_config.continueOnError) {
                // Stop test execution on failure
                emit logMessage("WARNING", QString("Step %1 failed: %2")
                               .arg(stepIndex + 1)
                               .arg(stepResult.resultMessage));
                break;
            }
        } else if (stepResult.status == TestStatus::Skipped) {
            result.skippedSteps++;
        }
        
        emit stepCompleted(testCase.id, stepIndex, stepResult);
        
        stepIndex++;
        
        // Add delay between steps
        if (m_config.stepDelayMs > 0) {
            QThread::msleep(m_config.stepDelayMs);
        }
    }
    
    result.endTime = QDateTime::currentDateTime();
    result.durationMs = m_testTimer.elapsed();
    
    if (m_stopRequested) {
        result.status = TestStatus::Skipped;
        result.statusMessage = "Stopped by user";
    } else if (overallSuccess) {
        result.status = TestStatus::Passed;
        result.statusMessage = QString("All %1 steps passed").arg(result.passedSteps);
    } else if (result.failedSteps > 0) {
        result.status = TestStatus::Failed;
        result.statusMessage = QString("%1 of %2 steps failed")
                              .arg(result.failedSteps)
                              .arg(result.totalSteps);
    } else {
        result.status = TestStatus::Error;
        result.statusMessage = "Unknown error";
    }
    
    return result;
}

TestStep TestExecutorEngine::executeStep(const TestStep& step, int stepIndex, const QString& testCaseId)
{
    TestStep result = step;
    result.status = TestStatus::Running;
    
    emit logMessage("DEBUG", QString("Executing step %1: %2 - %3")
                   .arg(stepIndex + 1)
                   .arg(TestStep::categoryToString(step.category))
                   .arg(step.command));
    
    // Execute the command via CommandRegistry with cancellation support
    auto& registry = CommandRegistry::instance();
    QVariantMap configMap = m_config.toVariantMap();
    
    // Determine step timeout: use step-specific timeout, or fall back to global default
    int stepTimeoutMs = step.parameters.value("timeout_ms", m_config.defaultTimeoutMs).toInt();
    if (stepTimeoutMs <= 0) stepTimeoutMs = m_config.defaultTimeoutMs;
    // Add a generous ceiling to allow command's own retry/timeout logic
    int hardTimeoutMs = stepTimeoutMs * 3 + 5000;
    
    try {
        // Run command with cancellation token
        auto future = QtConcurrent::run([&]() -> CommandResult {
            return registry.execute(step.command, step.parameters, configMap, &m_stopRequested);
        });
        
        // Wait with hard timeout
        QElapsedTimer stepDeadline;
        stepDeadline.start();
        
        while (!future.isFinished()) {
            if (stepDeadline.elapsed() > hardTimeoutMs) {
                m_stopRequested = true;   // force-cancel the handler
                future.waitForFinished();  // let it notice cancellation
                result.status = TestStatus::Error;
                result.resultMessage = QString("Step timed out after %1 ms").arg(hardTimeoutMs);
                result.durationMs = m_stepTimer.elapsed();
                m_stopRequested = false;   // reset for remaining steps
                return result;
            }
            QThread::msleep(10);
        }
        
        CommandResult cmdResult = future.result();
        
        result.durationMs = m_stepTimer.elapsed();
        result.responseData = cmdResult.responseData;
        
        if (cmdResult.success) {
            result.status = TestStatus::Passed;
            result.resultMessage = cmdResult.message;
        } else {
            result.status = TestStatus::Failed;
            result.resultMessage = cmdResult.message;
        }
        
    } catch (const std::exception& e) {
        result.status = TestStatus::Error;
        result.resultMessage = QString("Exception: %1").arg(e.what());
        result.durationMs = m_stepTimer.elapsed();
    }
    
    Q_UNUSED(testCaseId)
    return result;
}

void TestExecutorEngine::initializeCommunication()
{
    // Initialize serial port from configuration
    auto& serialMgr = SerialPortManager::instance();
    
    // Configure serial port from TestConfiguration
    SerialPortConfig serialConfig;
    serialConfig.portName = m_config.serialPort;
    serialConfig.baudRate = m_config.serialBaudRate;
    serialConfig.dataBits = SerialPortConfig::dataBitsFromInt(m_config.serialDataBits);
    serialConfig.stopBits = SerialPortConfig::stopBitsFromInt(m_config.serialStopBits);
    serialConfig.parity = SerialPortConfig::parityFromString(m_config.serialParity);
    serialConfig.readTimeoutMs = m_config.serialTimeoutMs;
    serialConfig.writeTimeoutMs = m_config.serialTimeoutMs;
    
    // Store the configuration (port will be opened on first use)
    serialMgr.setPortConfig(m_config.serialPort, serialConfig);
    
    emit logMessage("INFO", QString("Serial port configured: %1 @ %2 baud")
                            .arg(m_config.serialPort)
                            .arg(m_config.serialBaudRate));
    
    // TODO: Initialize CAN interface
    // TODO: Initialize power supply
    emit logMessage("INFO", "Communication interfaces initialized");
}

void TestExecutorEngine::syncFromHWConfig()
{
    auto& hwConfig = HWConfigManager::instance();

    // Sync primary serial port (Debug Port 1) into TestConfiguration
    auto serialCfg = hwConfig.serialDebugPort(0);
    if (!serialCfg.serial.portName.isEmpty()) {
        m_config.serialPort = serialCfg.serial.portName;
        m_config.serialBaudRate = serialCfg.serial.baudRate;
        m_config.serialDataBits = static_cast<int>(serialCfg.serial.dataBits);
        m_config.serialTimeoutMs = serialCfg.serial.readTimeoutMs;
    }

    // Sync primary CAN port (CAN 1) into TestConfiguration
    auto canCfg = hwConfig.canPort(0);
    if (!canCfg.customName.isEmpty()) {
        m_config.canInterface = canCfg.interfaceType;
        m_config.canChannel = canCfg.channel;
        m_config.canBitrate = canCfg.bitrate;
        m_config.canDataBitrate = canCfg.fdDataBitrate;
        m_config.canFdEnabled = canCfg.fdEnabled;
    }

    // Also push to SerialPortManager so connections use latest settings
    hwConfig.applyToSerialManager();
}

void TestExecutorEngine::cleanupCommunication()
{
    // Close serial ports
    auto& serialMgr = SerialPortManager::instance();
    serialMgr.closeAllPorts();
    
    emit logMessage("INFO", "Serial ports closed");
    
    // TODO: Close CAN interface
    // TODO: Safe shutdown power supply
    emit logMessage("INFO", "Communication interfaces closed");
}

} // namespace TestExecutor
