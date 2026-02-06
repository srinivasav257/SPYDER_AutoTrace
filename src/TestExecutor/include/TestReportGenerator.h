#pragma once
/**
 * @file TestReportGenerator.h
 * @brief HTML report generator for test results.
 * 
 * Generates professional HTML reports similar to pytest-html format:
 * - Summary statistics with pass/fail counts
 * - Detailed results table with all columns
 * - Collapsible test details
 * - Requirement and JIRA links
 * - Execution timestamps and durations
 * - Styling consistent with automotive industry standards
 */

#include "TestDataModels.h"
#include <QString>
#include <QJsonObject>

namespace TestExecutor {

/**
 * @brief Report format options
 */
enum class ReportFormat {
    HTML,       ///< Full HTML report with styling
    HTMLSimple, ///< Minimal HTML for embedding
    JSON,       ///< JSON data export
    CSV,        ///< CSV spreadsheet export
    XML         ///< JUnit XML format
};

/**
 * @brief Report customization options
 */
struct ReportOptions
{
    QString title = "Test Execution Report";
    QString subtitle;
    QString projectName;
    QString environment;
    QString testerName;
    QString buildVersion;
    
    bool includePassedTests = true;
    bool includeFailedTests = true;
    bool includeSkippedTests = true;
    bool includeStepDetails = true;
    bool includeLogs = true;
    bool includeTimestamps = true;
    bool includeLinks = true;
    
    QString logoPath;           ///< Path to logo image (embedded as base64)
    QString customCss;          ///< Additional CSS styles
    QString headerHtml;         ///< Custom header HTML
    QString footerHtml;         ///< Custom footer HTML
};

/**
 * @brief Generates test execution reports in various formats.
 */
class TestReportGenerator
{
public:
    /**
     * @brief Generate a report from a test session
     * @param session The completed test session
     * @param outputPath Path to save the report
     * @param format Report format
     * @param options Customization options
     * @return true if successful
     */
    static bool generateReport(const TestSession& session,
                               const QString& outputPath,
                               ReportFormat format = ReportFormat::HTML,
                               const ReportOptions& options = ReportOptions());

    /**
     * @brief Generate HTML report
     */
    static QString generateHtmlReport(const TestSession& session,
                                      const ReportOptions& options = ReportOptions());
    
    /**
     * @brief Generate JSON report
     */
    static QJsonObject generateJsonReport(const TestSession& session);
    
    /**
     * @brief Generate CSV report
     */
    static QString generateCsvReport(const TestSession& session);
    
    /**
     * @brief Generate JUnit XML report (for CI/CD integration)
     */
    static QString generateJunitXmlReport(const TestSession& session);

    /**
     * @brief Get default output filename with timestamp
     */
    static QString defaultFilename(ReportFormat format);

private:
    // HTML generation helpers
    static QString generateHtmlHead(const ReportOptions& options);
    static QString generateHtmlSummary(const TestSession& session, const ReportOptions& options);
    static QString generateHtmlResultsTable(const TestSession& session, const ReportOptions& options);
    static QString generateHtmlTestDetails(const TestResult& result, const ReportOptions& options);
    static QString generateHtmlStepRow(const TestStep& step);
    static QString generateHtmlStyles();
    static QString generateHtmlScripts();
    static QString statusToCssClass(TestStatus status);
    static QString statusToIcon(TestStatus status);
    static QString formatDuration(qint64 ms);
    static QString escapeHtml(const QString& text);
    static QString embedImage(const QString& imagePath);
};

} // namespace TestExecutor
