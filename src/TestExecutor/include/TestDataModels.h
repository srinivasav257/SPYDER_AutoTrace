#pragma once
/**
 * @file TestDataModels.h
 * @brief Core data models for the Test Executor framework.
 * 
 * This file defines the fundamental data structures used throughout
 * the test automation system:
 * - TestStep: Individual test action with parameters
 * - TestCase: Collection of steps with metadata
 * - TestResult: Execution outcome for a single test
 * - TestSession: Complete test run with all results
 */

#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

namespace TestExecutor {

//=============================================================================
// Enumerations
//=============================================================================

/**
 * @brief Status of a test execution
 */
enum class TestStatus {
    NotRun,     ///< Test has not been executed yet
    Running,    ///< Test is currently executing
    Passed,     ///< Test completed successfully
    Failed,     ///< Test failed (assertion failure)
    Error,      ///< Test encountered an error (exception/timeout)
    Skipped     ///< Test was skipped (precondition not met)
};

/**
 * @brief Category of test command/step
 */
enum class CommandCategory {
    Serial,     ///< Serial/UART commands (ManDiag, etc.)
    CAN,        ///< CAN bus communication
    Power,      ///< Power supply control
    Flow,       ///< Flow control (wait, loop, condition)
    Validation, ///< Response validation/assertion
    System,     ///< System commands (file, log, etc.)
    mdEOL       ///< Manufacturing Diagnostics via UART (End-of-Line)
};

//=============================================================================
// TestStep - Individual test action
//=============================================================================

/**
 * @brief Represents a single step in a test case.
 * 
 * Each step corresponds to one predefined function call with configurable
 * parameters. Steps are executed sequentially within a test case.
 * 
 * Example JSON:
 * @code
 * {
 *   "id": "step_001",
 *   "order": 1,
 *   "category": "Serial",
 *   "command": "Enter_MD_Session",
 *   "parameters": {
 *     "session_type": "diagnostic",
 *     "timeout_ms": 5000
 *   },
 *   "description": "Enter manufacturing diagnostic session",
 *   "enabled": true,
 *   "continueOnFail": false
 * }
 * @endcode
 */
struct TestStep
{
    QString id;                     ///< Unique step identifier (UUID)
    int order = 0;                  ///< Execution order (1-based)
    CommandCategory category;       ///< Command category
    QString command;                ///< Command name (from CommandRegistry)
    QVariantMap parameters;         ///< Command parameters
    QString description;            ///< Human-readable description
    bool enabled = true;            ///< Whether step is enabled
    bool continueOnFail = false;    ///< Continue to next step even if this fails
    
    // Execution results (populated during execution)
    TestStatus status = TestStatus::NotRun;
    QString resultMessage;          ///< Result/error message
    qint64 durationMs = 0;          ///< Execution duration in milliseconds
    QVariantMap responseData;       ///< Data returned by the command
    
    // Serialization
    QJsonObject toJson() const;
    static TestStep fromJson(const QJsonObject& json);
    
    // Helpers
    bool isValid() const { return !id.isEmpty() && !command.isEmpty(); }
    static QString generateId() { return QUuid::createUuid().toString(QUuid::WithoutBraces); }
    static QString categoryToString(CommandCategory cat);
    static CommandCategory categoryFromString(const QString& str);
};

//=============================================================================
// TestCase - Collection of steps with metadata
//=============================================================================

/**
 * @brief Represents a complete test case with metadata and steps.
 * 
 * A test case contains:
 * - Mandatory metadata (name, description, requirement ID, JIRA ticket)
 * - Ordered list of test steps
 * - Optional tags and configuration
 * 
 * Example JSON:
 * @code
 * {
 *   "id": "TC_EOL_001",
 *   "name": "EOL Boot Software ID Read",
 *   "description": "Verify boot software ID can be read via ManDiag",
 *   "requirementId": "REQ-DIAG-001",
 *   "requirementLink": "https://polarion.example.com/wi/REQ-DIAG-001",
 *   "jiraTicket": "HPCC-1234",
 *   "jiraLink": "https://jira.example.com/browse/HPCC-1234",
 *   "tags": ["EOL", "ManDiag", "Smoke"],
 *   "priority": 1,
 *   "steps": [...]
 * }
 * @endcode
 */
struct TestCase
{
    // === Mandatory Fields ===
    QString id;                     ///< Unique test case ID (e.g., TC_EOL_001)
    QString name;                   ///< Human-readable test name
    QString description;            ///< Detailed description
    QString requirementId;          ///< Requirement ID (e.g., REQ-DIAG-001)
    QString jiraTicket;             ///< JIRA ticket number (e.g., HPCC-1234)

    // === Optional Fields ===
    QString requirementLink;        ///< Full URL to requirement (Polarion, etc.)
    QString jiraLink;               ///< Full URL to JIRA ticket
    QStringList tags;               ///< Tags for filtering/grouping
    int priority = 5;               ///< Priority (1=highest, 10=lowest)
    QString author;                 ///< Test author
    QDateTime createdDate;          ///< Creation date
    QDateTime modifiedDate;         ///< Last modification date
    QString component;              ///< Group/component being tested
    QString feature;                ///< Feature within the group/component
    
    // === Test Steps ===
    QVector<TestStep> steps;        ///< Ordered list of test steps
    
    // === Configuration ===
    QVariantMap config;             ///< Test-specific configuration overrides
    int timeoutMs = 60000;          ///< Overall test timeout (default: 60s)
    bool enabled = true;            ///< Whether test is enabled
    
    // Serialization
    QJsonObject toJson() const;
    static TestCase fromJson(const QJsonObject& json);
    
    // Helpers
    bool isValid() const;
    int stepCount() const { return steps.size(); }
    int enabledStepCount() const;
    static QString generateId(const QString& prefix = "TC");
};

//=============================================================================
// TestResult - Execution outcome
//=============================================================================

/**
 * @brief Result of executing a single test case.
 */
struct TestResult
{
    QString testCaseId;             ///< Reference to the test case
    QString testCaseName;           ///< Test case name (for reporting)
    TestStatus status = TestStatus::NotRun;
    QString statusMessage;          ///< Overall status message
    
    QDateTime startTime;            ///< When execution started
    QDateTime endTime;              ///< When execution ended
    qint64 durationMs = 0;          ///< Total duration in milliseconds
    
    int totalSteps = 0;             ///< Total number of steps
    int passedSteps = 0;            ///< Number of passed steps
    int failedSteps = 0;            ///< Number of failed steps
    int skippedSteps = 0;           ///< Number of skipped steps
    
    QVector<TestStep> stepResults;  ///< Detailed results per step
    
    // Traceability
    QString requirementId;          ///< Requirement ID from test case
    QString requirementLink;        ///< Requirement link from test case
    QString jiraTicket;             ///< JIRA ticket from test case
    QString jiraLink;               ///< JIRA link from test case
    QString logOutput;              ///< Combined log output for this test
    QString screenshotPath;         ///< Optional screenshot path captured on failure
    
    // Serialization
    QJsonObject toJson() const;
    static TestResult fromJson(const QJsonObject& json);
    
    // Helpers
    static QString statusToString(TestStatus status);
    static TestStatus statusFromString(const QString& str);
};

//=============================================================================
// TestSession - Complete test run
//=============================================================================

/**
 * @brief Complete test execution session with all results.
 * 
 * Represents a single test run with metadata, configuration used,
 * and results for all executed tests.
 */
struct TestSession
{
    QString id;                     ///< Unique session ID
    QString name;                   ///< Session name/description
    
    QDateTime startTime;            ///< Session start time
    QDateTime endTime;              ///< Session end time
    qint64 durationMs = 0;          ///< Total duration
    
    // Summary counts
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    int errorTests = 0;
    int skippedTests = 0;
    
    // Configuration used
    QVariantMap configuration;      ///< Global configuration snapshot
    QString environment;            ///< Environment info (HW version, etc.)
    
    // Results
    QVector<TestResult> results;    ///< Results for each test
    
    // Serialization
    QJsonObject toJson() const;
    static TestSession fromJson(const QJsonObject& json);
    
    // Helpers
    double passRate() const { return totalTests > 0 ? (100.0 * passedTests / totalTests) : 0.0; }
    static QString generateId() { return QUuid::createUuid().toString(QUuid::WithoutBraces); }
};

//=============================================================================
// TestSuite - Grouping of test cases
//=============================================================================

/**
 * @brief Groups related test cases together.
 */
struct TestSuite
{
    QString id;                     ///< Unique suite ID
    QString name;                   ///< Suite name
    QString description;            ///< Suite description
    QString component;              ///< Component being tested
    QStringList testCaseIds;        ///< IDs of test cases in this suite
    
    QJsonObject toJson() const;
    static TestSuite fromJson(const QJsonObject& json);
};

} // namespace TestExecutor
