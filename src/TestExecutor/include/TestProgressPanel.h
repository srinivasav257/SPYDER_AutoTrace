#pragma once
/**
 * @file TestProgressPanel.h
 * @brief Test Progress panel for monitoring test execution.
 * 
 * Features:
 * - Real-time execution progress table
 * - Columns: #, Test Name, Description, Duration, Result, Requirement, JIRA
 * - Color-coded results (green=pass, red=fail, yellow=running)
 * - Export results to HTML report
 * - Live log viewer
 * - Execution controls (Run, Pause, Stop)
 */

#include "TestDataModels.h"
#include <QWidget>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>

namespace TestExecutor {

//=============================================================================
// TestProgressPanel
//=============================================================================

/**
 * @brief Panel for monitoring test execution progress and results.
 */
class TestProgressPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TestProgressPanel(QWidget* parent = nullptr);
    ~TestProgressPanel() override;

    /**
     * @brief Clear all results
     */
    void clear();
    
    /**
     * @brief Get current session results
     */
    TestSession currentSession() const;

public slots:
    // === Session Handlers ===
    void onSessionStarted(const QString& sessionId, int totalTests);
    void onSessionCompleted(const TestSession& session);
    
    // === Test Handlers ===
    void onTestStarted(const QString& testCaseId, const QString& testName, 
                       int testIndex, int totalTests);
    void onTestCompleted(const TestResult& result);
    void onTestProgress(const QString& testCaseId, int stepsCompleted, int totalSteps);
    
    // === Step Handlers ===
    void onStepStarted(const QString& testCaseId, int stepIndex, const QString& description);
    void onStepCompleted(const QString& testCaseId, int stepIndex, const TestStep& stepResult);
    
    // === Log Handlers ===
    void onLogMessage(const QString& level, const QString& message);
    void onCommunicationTrace(const QString& direction, const QString& interface, const QString& data);
    
    // === State Handlers ===
    void onStateChanged(ExecutorState newState);

signals:
    /**
     * @brief Emitted when user clicks Run button
     */
    void runRequested();
    
    /**
     * @brief Emitted when user clicks Pause button
     */
    void pauseRequested();
    
    /**
     * @brief Emitted when user clicks Stop button
     */
    void stopRequested();
    
    /**
     * @brief Emitted when user clicks Export Report
     */
    void exportReportRequested();
    
    /**
     * @brief Emitted when user double-clicks a result row
     */
    void resultDoubleClicked(const QString& testCaseId);

private slots:
    void onRunClicked();
    void onPauseClicked();
    void onStopClicked();
    void onExportClicked();
    void onClearClicked();
    void onResultTableDoubleClicked(int row, int column);
    void updateElapsedTime();

private:
    void setupUi();
    void setupConnections();
    void connectToEngine();
    void updateSummary();
    void setRowStatus(int row, TestStatus status);
    int findRowByTestId(const QString& testCaseId) const;

    // === Results Table ===
    QTableWidget* m_resultsTable = nullptr;
    
    // === Log Viewer ===
    QPlainTextEdit* m_logViewer = nullptr;
    
    // === Progress ===
    QProgressBar* m_overallProgress = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_elapsedLabel = nullptr;
    QLabel* m_currentTestLabel = nullptr;
    
    // === Summary ===
    QLabel* m_summaryLabel = nullptr;
    QLabel* m_passedLabel = nullptr;
    QLabel* m_failedLabel = nullptr;
    QLabel* m_skippedLabel = nullptr;
    
    // === Controls ===
    QPushButton* m_btnRun = nullptr;
    QPushButton* m_btnPause = nullptr;
    QPushButton* m_btnStop = nullptr;
    QPushButton* m_btnExport = nullptr;
    QPushButton* m_btnClear = nullptr;
    
    // === State ===
    TestSession m_session;
    QTimer* m_elapsedTimer = nullptr;
    QElapsedTimer m_elapsed;
    bool m_isRunning = false;
};

/**
 * @brief Factory function to create the panel for PanelRegistry
 */
QWidget* createTestProgressPanel(QWidget* parent);

} // namespace TestExecutor
