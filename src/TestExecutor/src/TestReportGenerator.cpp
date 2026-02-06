/**
 * @file TestReportGenerator.cpp
 * @brief Implementation of Test Report Generator (HTML, JSON, CSV, XML).
 */

#include "TestReportGenerator.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDateTime>
#include <QApplication>

namespace TestExecutor {

bool TestReportGenerator::generateReport(const TestSession& session,
                                          const QString& filePath,
                                          ReportFormat format,
                                          const ReportOptions& options)
{
    switch (format) {
        case ReportFormat::HTML:
            return generateHtmlReport(session, filePath, options);
        case ReportFormat::JSON:
            return generateJsonReport(session, filePath, options);
        case ReportFormat::CSV:
            return generateCsvReport(session, filePath, options);
        case ReportFormat::XML:
            return generateXmlReport(session, filePath, options);
    }
    return false;
}

QString TestReportGenerator::defaultFilename(ReportFormat format)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    
    switch (format) {
        case ReportFormat::HTML:
            return QString("TestReport_%1.html").arg(timestamp);
        case ReportFormat::JSON:
            return QString("TestReport_%1.json").arg(timestamp);
        case ReportFormat::CSV:
            return QString("TestReport_%1.csv").arg(timestamp);
        case ReportFormat::XML:
            return QString("TestReport_%1.xml").arg(timestamp);
    }
    return QString("TestReport_%1.txt").arg(timestamp);
}

bool TestReportGenerator::generateHtmlReport(const TestSession& session,
                                              const QString& filePath,
                                              const ReportOptions& options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // Calculate statistics
    double passRate = session.totalTests > 0 
        ? (100.0 * session.passedTests / session.totalTests) : 0;
    
    qint64 totalDuration = 0;
    for (const auto& result : session.results) {
        totalDuration += result.durationMs;
    }
    
    // HTML Header with embedded CSS (similar to pytest-html)
    out << R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" << options.projectName << R"( - Test Report</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { 
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
            line-height: 1.6;
            color: #333;
            background: #f5f5f5;
        }
        .container { max-width: 1400px; margin: 0 auto; padding: 20px; }
        
        /* Header */
        .header {
            background: linear-gradient(135deg, #1a237e 0%, #283593 100%);
            color: white;
            padding: 30px;
            border-radius: 8px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .header h1 { font-size: 2rem; margin-bottom: 10px; }
        .header .meta { opacity: 0.9; font-size: 0.9rem; }
        
        /* Summary Cards */
        .summary { 
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 20px;
        }
        .card {
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            text-align: center;
        }
        .card .value { font-size: 2rem; font-weight: bold; }
        .card .label { color: #666; font-size: 0.9rem; text-transform: uppercase; }
        .card.passed .value { color: #4caf50; }
        .card.failed .value { color: #f44336; }
        .card.skipped .value { color: #ff9800; }
        .card.total .value { color: #2196f3; }
        
        /* Progress Bar */
        .progress-container {
            background: white;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .progress-bar {
            display: flex;
            height: 30px;
            border-radius: 4px;
            overflow: hidden;
            background: #e0e0e0;
        }
        .progress-bar .passed { background: #4caf50; }
        .progress-bar .failed { background: #f44336; }
        .progress-bar .skipped { background: #ff9800; }
        
        /* Results Table */
        .results-table {
            background: white;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .results-table table {
            width: 100%;
            border-collapse: collapse;
        }
        .results-table th {
            background: #37474f;
            color: white;
            padding: 15px 10px;
            text-align: left;
            font-weight: 500;
        }
        .results-table td {
            padding: 12px 10px;
            border-bottom: 1px solid #e0e0e0;
        }
        .results-table tr:hover { background: #f5f5f5; }
        .results-table tr:nth-child(even) { background: #fafafa; }
        .results-table tr:nth-child(even):hover { background: #f0f0f0; }
        
        /* Status Badges */
        .status {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 20px;
            font-size: 0.8rem;
            font-weight: bold;
            text-transform: uppercase;
        }
        .status.passed { background: #e8f5e9; color: #2e7d32; }
        .status.failed { background: #ffebee; color: #c62828; }
        .status.error { background: #fff3e0; color: #e65100; }
        .status.skipped { background: #fff8e1; color: #f57f17; }
        .status.running { background: #e3f2fd; color: #1565c0; }
        
        /* Links */
        a { color: #1976d2; text-decoration: none; }
        a:hover { text-decoration: underline; }
        
        /* Footer */
        .footer {
            text-align: center;
            padding: 20px;
            color: #666;
            font-size: 0.85rem;
        }
        
        /* Collapsible Details */
        .details-toggle {
            cursor: pointer;
            color: #1976d2;
            font-size: 0.9rem;
        }
        .step-details {
            display: none;
            background: #f5f5f5;
            padding: 10px;
            margin-top: 10px;
            border-radius: 4px;
            font-family: monospace;
            font-size: 0.85rem;
        }
        .step-details.visible { display: block; }
    </style>
</head>
<body>
    <div class="container">
)";

    // Header Section
    out << "        <div class=\"header\">\n";
    out << "            <h1>" << options.projectName.toHtmlEscaped() << " - Test Report</h1>\n";
    out << "            <div class=\"meta\">\n";
    out << "                <p>Generated: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "</p>\n";
    out << "                <p>Environment: " << options.environment.toHtmlEscaped() << "</p>\n";
    out << "                <p>Session ID: " << session.id.toHtmlEscaped() << "</p>\n";
    out << "            </div>\n";
    out << "        </div>\n\n";
    
    // Summary Cards
    out << "        <div class=\"summary\">\n";
    out << "            <div class=\"card total\"><div class=\"value\">" << session.totalTests 
        << "</div><div class=\"label\">Total Tests</div></div>\n";
    out << "            <div class=\"card passed\"><div class=\"value\">" << session.passedTests 
        << "</div><div class=\"label\">Passed</div></div>\n";
    out << "            <div class=\"card failed\"><div class=\"value\">" << session.failedTests 
        << "</div><div class=\"label\">Failed</div></div>\n";
    out << "            <div class=\"card skipped\"><div class=\"value\">" << session.skippedTests 
        << "</div><div class=\"label\">Skipped</div></div>\n";
    out << "            <div class=\"card\"><div class=\"value\">" << QString::number(passRate, 'f', 1) 
        << "%</div><div class=\"label\">Pass Rate</div></div>\n";
    out << "            <div class=\"card\"><div class=\"value\">" << formatDuration(totalDuration) 
        << "</div><div class=\"label\">Duration</div></div>\n";
    out << "        </div>\n\n";
    
    // Progress Bar
    out << "        <div class=\"progress-container\">\n";
    out << "            <div class=\"progress-bar\">\n";
    if (session.totalTests > 0) {
        double passedPct = 100.0 * session.passedTests / session.totalTests;
        double failedPct = 100.0 * session.failedTests / session.totalTests;
        double skippedPct = 100.0 * session.skippedTests / session.totalTests;
        
        out << "                <div class=\"passed\" style=\"width: " << passedPct << "%;\"></div>\n";
        out << "                <div class=\"failed\" style=\"width: " << failedPct << "%;\"></div>\n";
        out << "                <div class=\"skipped\" style=\"width: " << skippedPct << "%;\"></div>\n";
    }
    out << "            </div>\n";
    out << "        </div>\n\n";
    
    // Results Table
    out << "        <div class=\"results-table\">\n";
    out << "            <table>\n";
    out << "                <thead>\n";
    out << "                    <tr>\n";
    out << "                        <th>#</th>\n";
    out << "                        <th>Test Name</th>\n";
    out << "                        <th>Duration</th>\n";
    out << "                        <th>Status</th>\n";
    out << "                        <th>Requirement</th>\n";
    out << "                        <th>JIRA</th>\n";
    out << "                        <th>Message</th>\n";
    out << "                    </tr>\n";
    out << "                </thead>\n";
    out << "                <tbody>\n";
    
    int index = 1;
    for (const auto& result : session.results) {
        QString statusClass = TestResult::statusToString(result.status).toLower();
        
        out << "                    <tr>\n";
        out << "                        <td>" << index++ << "</td>\n";
        out << "                        <td>" << result.testCaseName.toHtmlEscaped() << "</td>\n";
        out << "                        <td>" << formatDuration(result.durationMs) << "</td>\n";
        out << "                        <td><span class=\"status " << statusClass << "\">" 
            << TestResult::statusToString(result.status).toHtmlEscaped() << "</span></td>\n";
        
        // Requirement link
        out << "                        <td>";
        if (!result.requirementId.isEmpty()) {
            if (!options.requirementUrlTemplate.isEmpty()) {
                QString url = options.requirementUrlTemplate;
                url.replace("{id}", result.requirementId);
                out << "<a href=\"" << url.toHtmlEscaped() << "\" target=\"_blank\">" 
                    << result.requirementId.toHtmlEscaped() << "</a>";
            } else {
                out << result.requirementId.toHtmlEscaped();
            }
        }
        out << "</td>\n";
        
        // JIRA link
        out << "                        <td>";
        if (!result.jiraTicket.isEmpty()) {
            if (!options.jiraUrlTemplate.isEmpty()) {
                QString url = options.jiraUrlTemplate;
                url.replace("{id}", result.jiraTicket);
                out << "<a href=\"" << url.toHtmlEscaped() << "\" target=\"_blank\">" 
                    << result.jiraTicket.toHtmlEscaped() << "</a>";
            } else {
                out << result.jiraTicket.toHtmlEscaped();
            }
        }
        out << "</td>\n";
        
        out << "                        <td>" << result.statusMessage.toHtmlEscaped() << "</td>\n";
        out << "                    </tr>\n";
    }
    
    out << "                </tbody>\n";
    out << "            </table>\n";
    out << "        </div>\n\n";
    
    // Footer
    out << "        <div class=\"footer\">\n";
    out << "            <p>Report generated by SPYDER AutoTrace Test Executor</p>\n";
    out << "            <p>© " << QDate::currentDate().year() << " " << options.projectName.toHtmlEscaped() << "</p>\n";
    out << "        </div>\n";
    out << "    </div>\n";
    
    // JavaScript for interactive features
    out << R"(
    <script>
        function toggleDetails(id) {
            const el = document.getElementById(id);
            if (el) {
                el.classList.toggle('visible');
            }
        }
    </script>
</body>
</html>
)";
    
    file.close();
    return true;
}

bool TestReportGenerator::generateJsonReport(const TestSession& session,
                                              const QString& filePath,
                                              const ReportOptions& options)
{
    QJsonObject root;
    
    // Metadata
    QJsonObject metadata;
    metadata["projectName"] = options.projectName;
    metadata["environment"] = options.environment;
    metadata["generatedAt"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    metadata["generatedBy"] = "SPYDER AutoTrace Test Executor";
    root["metadata"] = metadata;
    
    // Session info
    QJsonObject sessionObj;
    sessionObj["id"] = session.id;
    sessionObj["startTime"] = session.startTime.toString(Qt::ISODate);
    sessionObj["endTime"] = session.endTime.toString(Qt::ISODate);
    sessionObj["totalTests"] = session.totalTests;
    sessionObj["passedTests"] = session.passedTests;
    sessionObj["failedTests"] = session.failedTests;
    sessionObj["skippedTests"] = session.skippedTests;
    
    qint64 totalDuration = 0;
    for (const auto& r : session.results) {
        totalDuration += r.durationMs;
    }
    sessionObj["totalDurationMs"] = totalDuration;
    
    if (session.totalTests > 0) {
        sessionObj["passRate"] = 100.0 * session.passedTests / session.totalTests;
    } else {
        sessionObj["passRate"] = 0.0;
    }
    
    root["session"] = sessionObj;
    
    // Test results
    QJsonArray resultsArray;
    for (const auto& result : session.results) {
        QJsonObject resultObj;
        resultObj["testCaseId"] = result.testCaseId;
        resultObj["testCaseName"] = result.testCaseName;
        resultObj["status"] = TestResult::statusToString(result.status);
        resultObj["statusMessage"] = result.statusMessage;
        resultObj["durationMs"] = result.durationMs;
        resultObj["startTime"] = result.startTime.toString(Qt::ISODate);
        resultObj["endTime"] = result.endTime.toString(Qt::ISODate);
        resultObj["requirementId"] = result.requirementId;
        resultObj["jiraTicket"] = result.jiraTicket;
        resultObj["logOutput"] = result.logOutput;
        
        if (!result.screenshotPath.isEmpty()) {
            resultObj["screenshotPath"] = result.screenshotPath;
        }
        
        // Step results
        QJsonArray stepsArray;
        for (const auto& step : result.stepResults) {
            QJsonObject stepObj;
            stepObj["id"] = step.id;
            stepObj["order"] = step.order;
            stepObj["description"] = step.description;
            stepObj["command"] = step.command;
            stepObj["status"] = TestResult::statusToString(step.status);
            stepObj["durationMs"] = step.durationMs;
            stepObj["resultMessage"] = step.resultMessage;
            
            QJsonObject paramsObj;
            for (auto it = step.parameters.begin(); it != step.parameters.end(); ++it) {
                paramsObj.insert(it.key(), QJsonValue::fromVariant(it.value()));
            }
            stepObj["parameters"] = paramsObj;
            
            stepsArray.append(stepObj);
        }
        resultObj["steps"] = stepsArray;
        
        resultsArray.append(resultObj);
    }
    root["results"] = resultsArray;
    
    // Write to file
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool TestReportGenerator::generateCsvReport(const TestSession& session,
                                             const QString& filePath,
                                             const ReportOptions& /*options*/)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // Header
    out << "Index,Test Case ID,Test Name,Status,Duration (ms),Start Time,End Time,"
        << "Requirement ID,JIRA Ticket,Message\n";
    
    int index = 1;
    for (const auto& result : session.results) {
        out << index++ << ","
            << escapeCsv(result.testCaseId) << ","
            << escapeCsv(result.testCaseName) << ","
            << escapeCsv(TestResult::statusToString(result.status)) << ","
            << result.durationMs << ","
            << escapeCsv(result.startTime.toString(Qt::ISODate)) << ","
            << escapeCsv(result.endTime.toString(Qt::ISODate)) << ","
            << escapeCsv(result.requirementId) << ","
            << escapeCsv(result.jiraTicket) << ","
            << escapeCsv(result.statusMessage) << "\n";
    }
    
    file.close();
    return true;
}

bool TestReportGenerator::generateXmlReport(const TestSession& session,
                                             const QString& filePath,
                                             const ReportOptions& options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<testsuites>\n";
    
    qint64 totalDuration = 0;
    for (const auto& r : session.results) {
        totalDuration += r.durationMs;
    }
    
    // TestSuite element
    out << "  <testsuite name=\"" << escapeXml(options.projectName) << "\"\n";
    out << "             tests=\"" << session.totalTests << "\"\n";
    out << "             failures=\"" << session.failedTests << "\"\n";
    out << "             skipped=\"" << session.skippedTests << "\"\n";
    out << "             time=\"" << QString::number(totalDuration / 1000.0, 'f', 3) << "\"\n";
    out << "             timestamp=\"" << session.startTime.toString(Qt::ISODate) << "\">\n";
    
    // Individual test cases
    for (const auto& result : session.results) {
        out << "    <testcase name=\"" << escapeXml(result.testCaseName) << "\"\n";
        out << "              classname=\"" << escapeXml(options.projectName) << "\"\n";
        out << "              time=\"" << QString::number(result.durationMs / 1000.0, 'f', 3) << "\">\n";
        
        if (result.status == TestStatus::Failed || result.status == TestStatus::Error) {
            QString type = result.status == TestStatus::Error ? "error" : "failure";
            out << "      <" << type << " message=\"" << escapeXml(result.statusMessage) << "\">\n";
            out << "        <![CDATA[" << result.logOutput << "]]>\n";
            out << "      </" << type << ">\n";
        } else if (result.status == TestStatus::Skipped) {
            out << "      <skipped message=\"" << escapeXml(result.statusMessage) << "\"/>\n";
        }
        
        // System output (log)
        if (!result.logOutput.isEmpty()) {
            out << "      <system-out><![CDATA[" << result.logOutput << "]]></system-out>\n";
        }
        
        // Properties for requirement and JIRA
        if (!result.requirementId.isEmpty() || !result.jiraTicket.isEmpty()) {
            out << "      <properties>\n";
            if (!result.requirementId.isEmpty()) {
                out << "        <property name=\"requirement\" value=\"" 
                    << escapeXml(result.requirementId) << "\"/>\n";
            }
            if (!result.jiraTicket.isEmpty()) {
                out << "        <property name=\"jira\" value=\"" 
                    << escapeXml(result.jiraTicket) << "\"/>\n";
            }
            out << "      </properties>\n";
        }
        
        out << "    </testcase>\n";
    }
    
    out << "  </testsuite>\n";
    out << "</testsuites>\n";
    
    file.close();
    return true;
}

QString TestReportGenerator::formatDuration(qint64 milliseconds)
{
    if (milliseconds < 1000) {
        return QString("%1 ms").arg(milliseconds);
    } else if (milliseconds < 60000) {
        return QString("%1 s").arg(milliseconds / 1000.0, 0, 'f', 2);
    } else {
        int mins = milliseconds / 60000;
        int secs = (milliseconds % 60000) / 1000;
        return QString("%1m %2s").arg(mins).arg(secs);
    }
}

QString TestReportGenerator::escapeCsv(const QString& value)
{
    if (value.contains(',') || value.contains('"') || value.contains('\n')) {
        QString escaped = value;
        escaped.replace("\"", "\"\"");
        return "\"" + escaped + "\"";
    }
    return value;
}

QString TestReportGenerator::escapeXml(const QString& value)
{
    QString result = value;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    result.replace("\"", "&quot;");
    result.replace("'", "&apos;");
    return result;
}

} // namespace TestExecutor
