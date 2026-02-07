/**
 * @file tst_TestDataModels.cpp
 * @brief Unit tests for TestDataModels JSON round-trip serialization.
 *
 * Verifies that every data structure (TestStep, TestCase, TestResult,
 * TestSession, TestSuite) survives toJson() → fromJson() without data loss.
 */

#include <gtest/gtest.h>
#include "TestDataModels.h"
#include <QJsonDocument>

using namespace TestExecutor;

// ============================================================================
// TestStep round-trip
// ============================================================================

TEST(TestDataModels, TestStep_JsonRoundTrip)
{
    TestStep step;
    step.id = "step_001";
    step.order = 3;
    step.category = CommandCategory::CAN;
    step.command = "CANHS_Tx";
    step.parameters = {{"can_id", "0x1A3"}, {"data", "AA BB CC"}};
    step.description = "Send CAN HS frame";
    step.enabled = true;
    step.continueOnFail = true;

    QJsonObject json = step.toJson();
    TestStep restored = TestStep::fromJson(json);

    EXPECT_EQ(restored.id, step.id);
    EXPECT_EQ(restored.order, step.order);
    EXPECT_EQ(restored.category, step.category);
    EXPECT_EQ(restored.command, step.command);
    EXPECT_EQ(restored.description, step.description);
    EXPECT_EQ(restored.enabled, step.enabled);
    EXPECT_EQ(restored.continueOnFail, step.continueOnFail);
    EXPECT_EQ(restored.parameters.value("can_id").toString(), "0x1A3");
    EXPECT_EQ(restored.parameters.value("data").toString(), "AA BB CC");
}

TEST(TestDataModels, TestStep_EmptyId_GetsGenerated)
{
    QJsonObject json;
    json["id"] = "";
    json["command"] = "flow_wait";
    json["category"] = "Flow";

    TestStep step = TestStep::fromJson(json);
    EXPECT_FALSE(step.id.isEmpty()) << "Empty id should be replaced with a generated UUID";
}

TEST(TestDataModels, TestStep_CategoryToStringRoundTrip)
{
    auto cats = {CommandCategory::Serial, CommandCategory::CAN,
                 CommandCategory::Power, CommandCategory::Flow,
                 CommandCategory::Validation, CommandCategory::System,
                 CommandCategory::mdEOL};

    for (auto cat : cats) {
        QString str = TestStep::categoryToString(cat);
        CommandCategory back = TestStep::categoryFromString(str);
        EXPECT_EQ(back, cat) << "Round-trip failed for category: "
                             << str.toStdString();
    }
}

// ============================================================================
// TestCase round-trip
// ============================================================================

TEST(TestDataModels, TestCase_JsonRoundTrip)
{
    TestCase tc;
    tc.id = "TC_EOL_001";
    tc.name = "EOL Boot Software ID Read";
    tc.description = "Verify boot software ID";
    tc.requirementId = "REQ-DIAG-001";
    tc.jiraTicket = "HPCC-1234";
    tc.requirementLink = "https://polarion.example.com/wi/REQ-DIAG-001";
    tc.jiraLink = "https://jira.example.com/browse/HPCC-1234";
    tc.tags = {"EOL", "ManDiag", "Smoke"};
    tc.priority = 1;
    tc.author = "TestAuthor";
    tc.createdDate = QDateTime::fromString("2025-01-15T10:30:00", Qt::ISODate);
    tc.modifiedDate = QDateTime::fromString("2025-06-20T14:00:00", Qt::ISODate);
    tc.component = "Infotainment";
    tc.feature = "Boot";
    tc.timeoutMs = 30000;
    tc.enabled = false;
    tc.config = {{"retry_count", 3}};

    // Add steps
    TestStep s1;
    s1.id = "s1";
    s1.order = 1;
    s1.category = CommandCategory::Serial;
    s1.command = "serial_enter_md_session";
    s1.parameters = {{"session_type", "manufacturing"}};
    s1.description = "Enter MFG session";
    s1.enabled = true;
    tc.steps.append(s1);

    TestStep s2;
    s2.id = "s2";
    s2.order = 2;
    s2.category = CommandCategory::Flow;
    s2.command = "flow_wait";
    s2.parameters = {{"duration_ms", 500}};
    s2.description = "Wait 500ms";
    s2.enabled = false;
    tc.steps.append(s2);

    QJsonObject json = tc.toJson();
    TestCase restored = TestCase::fromJson(json);

    EXPECT_EQ(restored.id, tc.id);
    EXPECT_EQ(restored.name, tc.name);
    EXPECT_EQ(restored.description, tc.description);
    EXPECT_EQ(restored.requirementId, tc.requirementId);
    EXPECT_EQ(restored.jiraTicket, tc.jiraTicket);
    EXPECT_EQ(restored.requirementLink, tc.requirementLink);
    EXPECT_EQ(restored.jiraLink, tc.jiraLink);
    EXPECT_EQ(restored.tags, tc.tags);
    EXPECT_EQ(restored.priority, tc.priority);
    EXPECT_EQ(restored.author, tc.author);
    EXPECT_EQ(restored.createdDate, tc.createdDate);
    EXPECT_EQ(restored.modifiedDate, tc.modifiedDate);
    EXPECT_EQ(restored.component, tc.component);
    EXPECT_EQ(restored.feature, tc.feature);
    EXPECT_EQ(restored.timeoutMs, tc.timeoutMs);
    EXPECT_EQ(restored.enabled, tc.enabled);
    EXPECT_EQ(restored.steps.size(), 2);
    EXPECT_EQ(restored.steps[0].command, "serial_enter_md_session");
    EXPECT_EQ(restored.steps[1].command, "flow_wait");
    EXPECT_EQ(restored.steps[1].enabled, false);
}

TEST(TestDataModels, TestCase_Validity)
{
    TestCase empty;
    EXPECT_FALSE(empty.isValid());

    TestCase valid;
    valid.id = "TC_001";
    valid.name = "Some Test";
    EXPECT_TRUE(valid.isValid());
}

TEST(TestDataModels, TestCase_EnabledStepCount)
{
    TestCase tc;
    tc.id = "TC_001";
    tc.name = "Test";

    TestStep s1; s1.id = "a"; s1.enabled = true;  s1.command = "x"; tc.steps.append(s1);
    TestStep s2; s2.id = "b"; s2.enabled = false; s2.command = "y"; tc.steps.append(s2);
    TestStep s3; s3.id = "c"; s3.enabled = true;  s3.command = "z"; tc.steps.append(s3);

    EXPECT_EQ(tc.enabledStepCount(), 2);
    EXPECT_EQ(tc.stepCount(), 3);
}

// ============================================================================
// TestResult round-trip
// ============================================================================

TEST(TestDataModels, TestResult_JsonRoundTrip)
{
    TestResult result;
    result.testCaseId = "TC_001";
    result.testCaseName = "Boot ID Read";
    result.status = TestStatus::Failed;
    result.statusMessage = "Step 2 assertion failed";
    result.startTime = QDateTime::fromString("2025-06-20T10:00:00", Qt::ISODate);
    result.endTime = QDateTime::fromString("2025-06-20T10:00:05", Qt::ISODate);
    result.durationMs = 5000;
    result.totalSteps = 3;
    result.passedSteps = 1;
    result.failedSteps = 1;
    result.skippedSteps = 1;
    result.requirementId = "REQ-001";
    result.requirementLink = "https://example.com/REQ-001";
    result.jiraTicket = "HPCC-999";
    result.jiraLink = "https://jira.example.com/HPCC-999";
    result.logOutput = "Some log output";
    result.screenshotPath = "/tmp/screenshot.png";

    // Add a step result
    TestStep sr;
    sr.id = "s1";
    sr.order = 1;
    sr.command = "flow_wait";
    sr.category = CommandCategory::Flow;
    sr.status = TestStatus::Passed;
    sr.resultMessage = "OK";
    sr.durationMs = 1000;
    sr.responseData = {{"key", "value"}};
    result.stepResults.append(sr);

    QJsonObject json = result.toJson();
    TestResult restored = TestResult::fromJson(json);

    EXPECT_EQ(restored.testCaseId, result.testCaseId);
    EXPECT_EQ(restored.testCaseName, result.testCaseName);
    EXPECT_EQ(restored.status, TestStatus::Failed);
    EXPECT_EQ(restored.statusMessage, result.statusMessage);
    EXPECT_EQ(restored.startTime, result.startTime);
    EXPECT_EQ(restored.endTime, result.endTime);
    EXPECT_EQ(restored.durationMs, 5000);
    EXPECT_EQ(restored.totalSteps, 3);
    EXPECT_EQ(restored.passedSteps, 1);
    EXPECT_EQ(restored.failedSteps, 1);
    EXPECT_EQ(restored.skippedSteps, 1);
    EXPECT_EQ(restored.requirementId, "REQ-001");
    EXPECT_EQ(restored.jiraTicket, "HPCC-999");
    EXPECT_EQ(restored.logOutput, "Some log output");
    EXPECT_EQ(restored.screenshotPath, "/tmp/screenshot.png");
    ASSERT_EQ(restored.stepResults.size(), 1);
    EXPECT_EQ(restored.stepResults[0].status, TestStatus::Passed);
    EXPECT_EQ(restored.stepResults[0].resultMessage, "OK");
    EXPECT_EQ(restored.stepResults[0].durationMs, 1000);
}

TEST(TestDataModels, TestResult_StatusStringRoundTrip)
{
    auto statuses = {TestStatus::NotRun, TestStatus::Running, TestStatus::Passed,
                     TestStatus::Failed, TestStatus::Error, TestStatus::Skipped};

    for (auto s : statuses) {
        QString str = TestResult::statusToString(s);
        TestStatus back = TestResult::statusFromString(str);
        EXPECT_EQ(back, s) << "Round-trip failed for status: " << str.toStdString();
    }
}

// ============================================================================
// TestSession round-trip
// ============================================================================

TEST(TestDataModels, TestSession_JsonRoundTrip)
{
    TestSession session;
    session.id = "session_001";
    session.name = "Nightly Run";
    session.startTime = QDateTime::fromString("2025-06-20T00:00:00", Qt::ISODate);
    session.endTime = QDateTime::fromString("2025-06-20T01:30:00", Qt::ISODate);
    session.durationMs = 5400000;
    session.totalTests = 10;
    session.passedTests = 7;
    session.failedTests = 2;
    session.errorTests = 1;
    session.skippedTests = 0;
    session.configuration = {{"env", "prod"}, {"retry", 2}};
    session.environment = "HW v2.1";

    TestResult r1;
    r1.testCaseId = "TC_001";
    r1.testCaseName = "Test 1";
    r1.status = TestStatus::Passed;
    session.results.append(r1);

    QJsonObject json = session.toJson();
    TestSession restored = TestSession::fromJson(json);

    EXPECT_EQ(restored.id, session.id);
    EXPECT_EQ(restored.name, session.name);
    EXPECT_EQ(restored.startTime, session.startTime);
    EXPECT_EQ(restored.endTime, session.endTime);
    EXPECT_EQ(restored.durationMs, 5400000);
    EXPECT_EQ(restored.totalTests, 10);
    EXPECT_EQ(restored.passedTests, 7);
    EXPECT_EQ(restored.failedTests, 2);
    EXPECT_EQ(restored.errorTests, 1);
    EXPECT_EQ(restored.skippedTests, 0);
    EXPECT_EQ(restored.environment, "HW v2.1");
    ASSERT_EQ(restored.results.size(), 1);
    EXPECT_EQ(restored.results[0].testCaseId, "TC_001");
    EXPECT_EQ(restored.results[0].status, TestStatus::Passed);
}

TEST(TestDataModels, TestSession_PassRate)
{
    TestSession session;
    session.totalTests = 0;
    EXPECT_DOUBLE_EQ(session.passRate(), 0.0);

    session.totalTests = 4;
    session.passedTests = 3;
    EXPECT_DOUBLE_EQ(session.passRate(), 75.0);
}

// ============================================================================
// TestSuite round-trip
// ============================================================================

TEST(TestDataModels, TestSuite_JsonRoundTrip)
{
    TestSuite suite;
    suite.id = "suite_001";
    suite.name = "EOL Suite";
    suite.description = "All EOL test cases";
    suite.component = "Infotainment";
    suite.testCaseIds = {"TC_001", "TC_002", "TC_003"};

    QJsonObject json = suite.toJson();
    TestSuite restored = TestSuite::fromJson(json);

    EXPECT_EQ(restored.id, suite.id);
    EXPECT_EQ(restored.name, suite.name);
    EXPECT_EQ(restored.description, suite.description);
    EXPECT_EQ(restored.component, suite.component);
    EXPECT_EQ(restored.testCaseIds, suite.testCaseIds);
}

// ============================================================================
// Edge cases
// ============================================================================

TEST(TestDataModels, TestStep_FromEmptyJson)
{
    QJsonObject empty;
    TestStep step = TestStep::fromJson(empty);
    // Should not crash, id should be generated
    EXPECT_FALSE(step.id.isEmpty());
    EXPECT_TRUE(step.command.isEmpty());
}

TEST(TestDataModels, TestCase_FromEmptyJson)
{
    QJsonObject empty;
    TestCase tc = TestCase::fromJson(empty);
    EXPECT_TRUE(tc.id.isEmpty());
    EXPECT_TRUE(tc.name.isEmpty());
    EXPECT_TRUE(tc.steps.isEmpty());
    EXPECT_EQ(tc.enabled, true);  // default
    EXPECT_EQ(tc.timeoutMs, 60000);  // default
}

TEST(TestDataModels, TestResult_FromEmptyJson)
{
    QJsonObject empty;
    TestResult result = TestResult::fromJson(empty);
    EXPECT_TRUE(result.testCaseId.isEmpty());
    EXPECT_EQ(result.status, TestStatus::NotRun);
    EXPECT_EQ(result.durationMs, 0);
}

TEST(TestDataModels, TestSession_FromEmptyJson)
{
    QJsonObject empty;
    TestSession session = TestSession::fromJson(empty);
    EXPECT_TRUE(session.id.isEmpty());
    EXPECT_EQ(session.totalTests, 0);
    EXPECT_DOUBLE_EQ(session.passRate(), 0.0);
}
