#pragma once
/**
 * @file TestExecutorEngine.h
 * @brief Engine for executing test cases.
 * 
 * The TestExecutorEngine:
 * - Runs test cases sequentially or selectively
 * - Emits signals for UI updates (progress, step completion)
 * - Manages communication interfaces (Serial, CAN)
 * - Handles test flow control (stop, pause, resume)
 */

#include "TestDataModels.h"
#include "CommandRegistry.h"
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <atomic>

namespace TestExecutor {

//=============================================================================
// ExecutorState
//=============================================================================

enum class ExecutorState {
    Idle,       ///< Not running
    Running,    ///< Executing tests
    Paused,     ///< Paused mid-execution
    Stopping    ///< Stop requested, finishing current step
};

//=============================================================================
// Global Configuration
//=============================================================================

/**
 * @brief Global test configuration settings
 */
struct TestConfiguration
{
    // Serial Port Settings
    QString serialPort = "COM3";
    int serialBaudRate = 115200;
    int serialDataBits = 8;
    int serialStopBits = 1;
    QString serialParity = "None";
    int serialTimeoutMs = 1000;
    
    // CAN Settings
    QString canInterface = "Vector";
    int canChannel = 0;
    int canBitrate = 500000;
    int canDataBitrate = 2000000;
    bool canFdEnabled = true;
    
    // Power Supply Settings
    QString powerSupplyType = "Keysight";
    QString powerSupplyPort = "USB0::...";
    double defaultVoltage = 12.0;
    double maxCurrent = 10.0;
    
    // Execution Settings
    bool stopOnFirstFailure = false;
    bool continueOnError = false;
    int stepDelayMs = 100;
    int defaultTimeoutMs = 5000;
    
    // Logging Settings
    QString logLevel = "INFO";
    QString logFilePath;
    bool logToFile = true;
    
    // Report Settings
    QString reportOutputPath;
    QString reportTemplate = "default";
    bool autoGenerateReport = true;
    
    // Convert to/from QVariantMap for command handlers
    QVariantMap toVariantMap() const;
    static TestConfiguration fromVariantMap(const QVariantMap& map);
    
    // JSON serialization
    QJsonObject toJson() const;
    static TestConfiguration fromJson(const QJsonObject& json);
};

//=============================================================================
// TestExecutorEngine
//=============================================================================

/**
 * @brief Engine that executes test cases.
 * 
 * Usage:
 * @code
 * auto& engine = TestExecutorEngine::instance();
 * 
 * // Connect to signals
 * connect(&engine, &TestExecutorEngine::testStarted, this, &MyWidget::onTestStarted);
 * connect(&engine, &TestExecutorEngine::stepCompleted, this, &MyWidget::onStepCompleted);
 * connect(&engine, &TestExecutorEngine::sessionCompleted, this, &MyWidget::onSessionCompleted);
 * 
 * // Run tests
 * QStringList testIds = {"TC_001", "TC_002", "TC_003"};
 * engine.runTests(testIds);
 * @endcode
 */
class TestExecutorEngine : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     */
    static TestExecutorEngine& instance();

    // === Configuration ===
    
    /**
     * @brief Get current configuration
     */
    const TestConfiguration& configuration() const { return m_config; }
    
    /**
     * @brief Set configuration
     */
    void setConfiguration(const TestConfiguration& config);
    
    /**
     * @brief Load configuration from file
     */
    bool loadConfiguration(const QString& filePath);
    
    /**
     * @brief Save configuration to file
     */
    bool saveConfiguration(const QString& filePath) const;

    // === State ===
    
    /**
     * @brief Get current engine state
     */
    ExecutorState state() const { return m_state; }
    
    /**
     * @brief Check if engine is running
     */
    bool isRunning() const { return m_state == ExecutorState::Running || m_state == ExecutorState::Paused; }
    
    /**
     * @brief Get current session (may be null)
     */
    const TestSession* currentSession() const { return m_currentSession.get(); }

    // === Execution Control ===
    
    /**
     * @brief Run a list of test cases
     * @param testCaseIds List of test case IDs to run
     */
    void runTests(const QStringList& testCaseIds);
    
    /**
     * @brief Run all test cases
     */
    void runAllTests();
    
    /**
     * @brief Run a single test case (for debugging)
     */
    void runSingleTest(const QString& testCaseId);
    
    /**
     * @brief Run a single step from a test case (for debugging)
     */
    void runSingleStep(const QString& testCaseId, int stepIndex);
    
    /**
     * @brief Pause execution after current step completes
     */
    void pause();
    
    /**
     * @brief Resume paused execution
     */
    void resume();
    
    /**
     * @brief Stop execution after current step completes
     */
    void stop();
    
    /**
     * @brief Emergency stop - interrupt immediately
     */
    void emergencyStop();

signals:
    // === Session Signals ===
    
    /**
     * @brief Emitted when a test session starts
     */
    void sessionStarted(const QString& sessionId, int totalTests);
    
    /**
     * @brief Emitted when a test session completes
     */
    void sessionCompleted(const TestSession& session);
    
    /**
     * @brief Emitted when execution state changes
     */
    void stateChanged(ExecutorState newState);

    // === Test Case Signals ===
    
    /**
     * @brief Emitted when a test case starts
     */
    void testStarted(const QString& testCaseId, const QString& testName, int testIndex, int totalTests);
    
    /**
     * @brief Emitted when a test case completes
     */
    void testCompleted(const TestResult& result);
    
    /**
     * @brief Emitted on test progress update
     */
    void testProgress(const QString& testCaseId, int stepsCompleted, int totalSteps);

    // === Step Signals ===
    
    /**
     * @brief Emitted when a step starts
     */
    void stepStarted(const QString& testCaseId, int stepIndex, const QString& stepDescription);
    
    /**
     * @brief Emitted when a step completes
     */
    void stepCompleted(const QString& testCaseId, int stepIndex, const TestStep& stepResult);
    
    // === Log Signals ===
    
    /**
     * @brief Emitted for log messages
     */
    void logMessage(const QString& level, const QString& message);
    
    /**
     * @brief Emitted for communication trace
     */
    void communicationTrace(const QString& direction, const QString& interface, const QString& data);

private slots:
    void executeTests();

private:
    TestExecutorEngine();
    ~TestExecutorEngine() override;
    TestExecutorEngine(const TestExecutorEngine&) = delete;
    TestExecutorEngine& operator=(const TestExecutorEngine&) = delete;

    void setState(ExecutorState state);
    TestResult executeTestCase(const TestCase& testCase);
    TestStep executeStep(const TestStep& step, int stepIndex, const QString& testCaseId);
    void initializeCommunication();
    void cleanupCommunication();

    TestConfiguration m_config;
    ExecutorState m_state = ExecutorState::Idle;
    std::unique_ptr<TestSession> m_currentSession;
    QStringList m_pendingTestIds;
    
    QThread* m_workerThread = nullptr;
    QMutex m_mutex;
    QWaitCondition m_pauseCondition;
    std::atomic<bool> m_stopRequested{false};
    std::atomic<bool> m_pauseRequested{false};
    
    QElapsedTimer m_sessionTimer;
    QElapsedTimer m_testTimer;
    QElapsedTimer m_stepTimer;
};

} // namespace TestExecutor
