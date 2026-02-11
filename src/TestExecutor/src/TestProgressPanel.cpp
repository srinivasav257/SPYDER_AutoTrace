/**
 * @file TestProgressPanel.cpp
 * @brief Implementation of Test Progress Panel.
 */

#include "TestProgressPanel.h"
#include "TestExecutorEngine.h"
#include "TestReportGenerator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QFontDatabase>
#include <QGroupBox>

namespace TestExecutor {

TestProgressPanel::TestProgressPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("testProgressPanel"));
    setupUi();
    setupConnections();
    connectToEngine();
}

TestProgressPanel::~TestProgressPanel() = default;

void TestProgressPanel::clear()
{
    // Clear content but keep 10 empty rows
    m_resultsTable->clearContents();
    m_resultsTable->setRowCount(10);
    m_logViewer->clear();
    m_overallProgress->setValue(0);
    m_session = TestSession();
    m_nextRow = 0;
}

TestSession TestProgressPanel::currentSession() const
{
    return m_session;
}

void TestProgressPanel::onSessionStarted(const QString& sessionId, int totalTests)
{
    clear();
    m_session.id = sessionId;
    m_session.totalTests = totalTests;
    m_session.startTime = QDateTime::currentDateTime();
    
    m_overallProgress->setMaximum(totalTests);
    m_overallProgress->setValue(0);
    m_isRunning = true;

    m_elapsed.start();
    m_elapsedTimer->start(1000);
    
    appendLog("INFO", QString("Session started: %1 tests").arg(totalTests));
}

void TestProgressPanel::onSessionCompleted(const TestSession& session)
{
    m_session = session;
    m_isRunning = false;
    m_elapsedTimer->stop();

    updateSummary();

    // Print test execution summary to log
    qint64 elapsed = m_elapsed.elapsed();
    int hours = elapsed / 3600000;
    int mins = (elapsed % 3600000) / 60000;
    int secs = (elapsed % 60000) / 1000;
    QString elapsedStr = QString("%1:%2:%3")
                         .arg(hours, 2, 10, QChar('0'))
                         .arg(mins, 2, 10, QChar('0'))
                         .arg(secs, 2, 10, QChar('0'));

    appendLog("INFO", QString(""));
    appendLog("INFO", QString("═══════════════════════════════════════════"));
    appendLog("INFO", QString("  TEST EXECUTION SUMMARY"));
    appendLog("INFO", QString("═══════════════════════════════════════════"));
    appendLog("INFO", QString("  Total Tests : %1").arg(session.totalTests));
    appendLog("INFO", QString("  Passed      : %1").arg(session.passedTests));
    appendLog("INFO", QString("  Failed      : %1").arg(session.failedTests));
    appendLog("INFO", QString("  Skipped     : %1").arg(session.skippedTests));
    appendLog("INFO", QString("  Elapsed     : %1").arg(elapsedStr));
    appendLog("INFO", QString("═══════════════════════════════════════════"));
    
    // Auto-export if configured
    auto& engine = TestExecutorEngine::instance();
    if (engine.configuration().autoGenerateReport) {
        QString filename = TestReportGenerator::defaultFilename(ReportFormat::HTML);
        QString outputPath = engine.configuration().reportOutputPath;
        if (!outputPath.isEmpty()) {
            filename = outputPath + "/" + filename;
        }
        
        if (TestReportGenerator::generateReport(session, filename)) {
            appendLog("INFO", "Report generated: " + filename);
        }
    }
}

void TestProgressPanel::onTestStarted(const QString& testCaseId, const QString& testName,
                                       int testIndex, int totalTests)
{
    Q_UNUSED(totalTests)

    int row = m_nextRow;

    // Expand table if we've filled all existing rows
    if (row >= m_resultsTable->rowCount()) {
        m_resultsTable->setRowCount(row + 1);
    }

    m_resultsTable->setItem(row, 0, new QTableWidgetItem(QString::number(testIndex + 1)));
    m_resultsTable->setItem(row, 1, new QTableWidgetItem(testName));
    m_resultsTable->setItem(row, 2, new QTableWidgetItem(""));  // Description
    m_resultsTable->setItem(row, 3, new QTableWidgetItem("--")); // Duration
    m_resultsTable->setItem(row, 4, new QTableWidgetItem("Running..."));
    m_resultsTable->setItem(row, 5, new QTableWidgetItem(""));  // Requirement
    m_resultsTable->setItem(row, 6, new QTableWidgetItem(""));  // JIRA

    // Store test case ID in user data
    m_resultsTable->item(row, 0)->setData(Qt::UserRole, testCaseId);

    setRowStatus(row, TestStatus::Running);
    m_nextRow++;

    m_overallProgress->setValue(testIndex);
    
    // Auto-scroll to new row
    m_resultsTable->scrollToBottom();
    
    appendLog("INFO", QString("Starting: %1").arg(testName));
}

void TestProgressPanel::onTestCompleted(const TestResult& result)
{
    int row = findRowByTestId(result.testCaseId);
    if (row < 0) return;
    
    // Update row
    m_resultsTable->item(row, 2)->setText(result.statusMessage);
    m_resultsTable->item(row, 3)->setText(QString("%1 ms").arg(result.durationMs));
    m_resultsTable->item(row, 4)->setText(TestResult::statusToString(result.status));
    m_resultsTable->item(row, 5)->setText(result.requirementId);
    m_resultsTable->item(row, 6)->setText(result.jiraTicket);
    
    setRowStatus(row, result.status);
    updateSummary();
    
    m_session.results.append(result);
}

void TestProgressPanel::onTestProgress(const QString& /*testCaseId*/, int stepsCompleted, int totalSteps)
{
    Q_UNUSED(stepsCompleted)
    Q_UNUSED(totalSteps)
}

void TestProgressPanel::onStepStarted(const QString& /*testCaseId*/, int stepIndex, const QString& description)
{
    appendLog("DEBUG", QString("  Step %1: %2").arg(stepIndex + 1).arg(description));
}

void TestProgressPanel::onStepCompleted(const QString& /*testCaseId*/, int stepIndex, const TestStep& stepResult)
{
    QString status = TestResult::statusToString(stepResult.status);
    QString duration = QString::number(stepResult.durationMs) + " ms";
    
    if (stepResult.status == TestStatus::Passed) {
        appendLog("DEBUG", QString("  ✓ Step %1: %2 (%3)").arg(stepIndex + 1).arg(status).arg(duration));
    } else if (stepResult.status == TestStatus::Failed || stepResult.status == TestStatus::Error) {
        appendLog("ERROR", QString("  ✗ Step %1: %2 - %3 (%4)")
                 .arg(stepIndex + 1)
                 .arg(status)
                 .arg(stepResult.resultMessage)
                 .arg(duration));
    }
}

void TestProgressPanel::onLogMessage(const QString& level, const QString& message)
{
    appendLog(level, message);
}

void TestProgressPanel::onCommunicationTrace(const QString& direction, const QString& interface, 
                                              const QString& data)
{
    QString arrow = direction == "TX" ? "→" : "←";
    appendLog("TRACE", QString("[%1] %2 %3").arg(interface, arrow, data));
}

void TestProgressPanel::onStateChanged(ExecutorState newState)
{
    switch (newState) {
        case ExecutorState::Idle:
            m_btnRun->setEnabled(true);
            m_btnPause->setEnabled(false);
            m_btnStop->setEnabled(false);
            break;

        case ExecutorState::Running:
            m_btnRun->setEnabled(false);
            m_btnPause->setEnabled(true);
            m_btnStop->setEnabled(true);
            break;

        case ExecutorState::Paused:
            m_btnRun->setEnabled(false);
            m_btnPause->setText("Resume");
            m_btnStop->setEnabled(true);
            break;

        case ExecutorState::Stopping:
            m_btnRun->setEnabled(false);
            m_btnPause->setEnabled(false);
            m_btnStop->setEnabled(false);
            break;
    }
}

void TestProgressPanel::onRunClicked()
{
    emit runRequested();
}

void TestProgressPanel::onPauseClicked()
{
    auto& engine = TestExecutorEngine::instance();
    
    if (engine.state() == ExecutorState::Paused) {
        engine.resume();
        m_btnPause->setText("Pause");
    } else {
        engine.pause();
        m_btnPause->setText("Resume");
    }
    
    emit pauseRequested();
}

void TestProgressPanel::onStopClicked()
{
    auto& engine = TestExecutorEngine::instance();
    engine.stop();
    emit stopRequested();
}

void TestProgressPanel::onExportClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this, "Export Report",
        TestReportGenerator::defaultFilename(ReportFormat::HTML),
        "HTML Report (*.html);;JSON Report (*.json);;CSV Report (*.csv);;All Files (*)");
    
    if (filePath.isEmpty()) return;
    
    ReportFormat format = ReportFormat::HTML;
    if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        format = ReportFormat::JSON;
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        format = ReportFormat::CSV;
    }
    
    ReportOptions options;
    options.projectName = "Infotainment Test Automation";
    options.environment = "Test Environment";
    
    if (TestReportGenerator::generateReport(m_session, filePath, format, options)) {
        QMessageBox::information(this, "Export Complete",
                                 "Report exported successfully:\n" + filePath);
    } else {
        QMessageBox::critical(this, "Export Failed",
                              "Failed to export report.");
    }
    
    emit exportReportRequested();
}

void TestProgressPanel::onClearClicked()
{
    clear();
}

void TestProgressPanel::onResultTableDoubleClicked(int row, int /*column*/)
{
    QTableWidgetItem* item = m_resultsTable->item(row, 0);
    if (item) {
        QString testCaseId = item->data(Qt::UserRole).toString();
        emit resultDoubleClicked(testCaseId);
    }
}

void TestProgressPanel::updateElapsedTime()
{
    // Elapsed timer keeps running to track total time for summary log
}

void TestProgressPanel::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);
    
    // === Control Bar ===
    auto* controlBar = new QHBoxLayout();
    
    m_btnRun = new QPushButton("▶ Run All", this);
    
    m_btnPause = new QPushButton("⏸ Pause", this);
    m_btnPause->setEnabled(false);
    
    m_btnStop = new QPushButton("⏹ Stop", this);
    m_btnStop->setEnabled(false);
    
    m_btnExport = new QPushButton("📄 Export Report", this);
    m_btnClear = new QPushButton("Clear", this);
    
    controlBar->addWidget(m_btnRun);
    controlBar->addWidget(m_btnPause);
    controlBar->addWidget(m_btnStop);
    controlBar->addSpacing(20);
    controlBar->addWidget(m_btnExport);
    controlBar->addWidget(m_btnClear);
    controlBar->addStretch();
    
    mainLayout->addLayout(controlBar);
    
    // === Progress Bar ===
    auto* progressLayout = new QHBoxLayout();

    m_overallProgress = new QProgressBar(this);
    m_overallProgress->setMinimum(0);
    m_overallProgress->setMaximum(100);
    m_overallProgress->setValue(0);
    m_overallProgress->setTextVisible(true);

    progressLayout->addWidget(new QLabel("Progress:", this));
    progressLayout->addWidget(m_overallProgress, 1);

    mainLayout->addLayout(progressLayout);
    
    // === Splitter for Table and Log ===
    auto* splitter = new QSplitter(Qt::Vertical, this);
    
    // Results Table
    m_resultsTable = new QTableWidget(splitter);
    m_resultsTable->setColumnCount(7);
    m_resultsTable->setHorizontalHeaderLabels({
        "Sl No.", "Test Name", "Description", "Duration", "Result", "Requirement", "JIRA"
    });
    m_resultsTable->horizontalHeader()->setStretchLastSection(false);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setAlternatingRowColors(false);
    m_resultsTable->setFrameShape(QFrame::NoFrame);
    m_resultsTable->setRowCount(10);

    // Log Viewer
    auto* logGroup = new QGroupBox("Execution Log", splitter);
    auto* logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(2, 2, 2, 2);

    m_logViewer = new QPlainTextEdit(logGroup);
    m_logViewer->setReadOnly(true);
    m_logViewer->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_logViewer->setMaximumBlockCount(10000); // Limit log size

    logLayout->addWidget(m_logViewer);

    splitter->addWidget(m_resultsTable);
    splitter->addWidget(logGroup);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
    
    // Timer for elapsed time
    m_elapsedTimer = new QTimer(this);
}

void TestProgressPanel::setupConnections()
{
    connect(m_btnRun, &QPushButton::clicked, this, &TestProgressPanel::onRunClicked);
    connect(m_btnPause, &QPushButton::clicked, this, &TestProgressPanel::onPauseClicked);
    connect(m_btnStop, &QPushButton::clicked, this, &TestProgressPanel::onStopClicked);
    connect(m_btnExport, &QPushButton::clicked, this, &TestProgressPanel::onExportClicked);
    connect(m_btnClear, &QPushButton::clicked, this, &TestProgressPanel::onClearClicked);
    
    connect(m_resultsTable, &QTableWidget::cellDoubleClicked, 
            this, &TestProgressPanel::onResultTableDoubleClicked);
    
    connect(m_elapsedTimer, &QTimer::timeout, this, &TestProgressPanel::updateElapsedTime);
}

void TestProgressPanel::connectToEngine()
{
    auto& engine = TestExecutorEngine::instance();
    
    connect(&engine, &TestExecutorEngine::sessionStarted, 
            this, &TestProgressPanel::onSessionStarted);
    connect(&engine, &TestExecutorEngine::sessionCompleted, 
            this, &TestProgressPanel::onSessionCompleted);
    connect(&engine, &TestExecutorEngine::testStarted, 
            this, &TestProgressPanel::onTestStarted);
    connect(&engine, &TestExecutorEngine::testCompleted, 
            this, &TestProgressPanel::onTestCompleted);
    connect(&engine, &TestExecutorEngine::testProgress, 
            this, &TestProgressPanel::onTestProgress);
    connect(&engine, &TestExecutorEngine::stepStarted, 
            this, &TestProgressPanel::onStepStarted);
    connect(&engine, &TestExecutorEngine::stepCompleted, 
            this, &TestProgressPanel::onStepCompleted);
    connect(&engine, &TestExecutorEngine::logMessage, 
            this, &TestProgressPanel::onLogMessage);
    connect(&engine, &TestExecutorEngine::communicationTrace, 
            this, &TestProgressPanel::onCommunicationTrace);
    connect(&engine, &TestExecutorEngine::stateChanged, 
            this, &TestProgressPanel::onStateChanged);
}

void TestProgressPanel::updateSummary()
{
    int passed = 0, failed = 0, skipped = 0;

    for (const auto& result : m_session.results) {
        switch (result.status) {
            case TestStatus::Passed: passed++; break;
            case TestStatus::Failed:
            case TestStatus::Error: failed++; break;
            case TestStatus::Skipped: skipped++; break;
            default: break;
        }
    }

    m_session.passedTests = passed;
    m_session.failedTests = failed;
    m_session.skippedTests = skipped;
}

void TestProgressPanel::setRowStatus(int row, TestStatus status)
{
    Q_UNUSED(status)
    
    for (int col = 0; col < m_resultsTable->columnCount(); ++col) {
        QTableWidgetItem* item = m_resultsTable->item(row, col);
        if (item) {
            item->setData(Qt::BackgroundRole, QVariant());
            item->setData(Qt::ForegroundRole, QVariant());
        }
    }
}

int TestProgressPanel::findRowByTestId(const QString& testCaseId) const
{
    for (int row = 0; row < m_resultsTable->rowCount(); ++row) {
        QTableWidgetItem* item = m_resultsTable->item(row, 0);
        if (item && item->data(Qt::UserRole).toString() == testCaseId) {
            return row;
        }
    }
    return -1;
}

void TestProgressPanel::appendLog(const QString& level, const QString& message)
{
    Q_UNUSED(level)

    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    m_logViewer->appendPlainText(QString("[%1] %2").arg(timestamp, message));
}

QWidget* createTestProgressPanel(QWidget* parent)
{
    return new TestProgressPanel(parent);
}

} // namespace TestExecutor
