/**
 * @file TestDataModels.cpp
 * @brief Implementation of test data model serialization.
 */

#include "TestDataModels.h"
#include <QJsonDocument>

namespace TestExecutor {

//=============================================================================
// TestStep Implementation
//=============================================================================

QString TestStep::categoryToString(CommandCategory cat)
{
    switch (cat) {
        case CommandCategory::Serial: return "Serial";
        case CommandCategory::CAN: return "CAN";
        case CommandCategory::Power: return "Power";
        case CommandCategory::Flow: return "Flow";
        case CommandCategory::Validation: return "Validation";
        case CommandCategory::System: return "System";
        case CommandCategory::mdEOL: return "mdEOL";
    }
    return "Unknown";
}

CommandCategory TestStep::categoryFromString(const QString& str)
{
    if (str == "Serial") return CommandCategory::Serial;
    if (str == "CAN") return CommandCategory::CAN;
    if (str == "Power") return CommandCategory::Power;
    if (str == "Flow") return CommandCategory::Flow;
    if (str == "Validation") return CommandCategory::Validation;
    if (str == "System") return CommandCategory::System;
    if (str == "mdEOL") return CommandCategory::mdEOL;
    return CommandCategory::Serial; // Default
}

QJsonObject TestStep::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["order"] = order;
    obj["category"] = categoryToString(category);
    obj["command"] = command;
    obj["parameters"] = QJsonObject::fromVariantMap(parameters);
    obj["description"] = description;
    obj["enabled"] = enabled;
    obj["continueOnFail"] = continueOnFail;
    return obj;
}

TestStep TestStep::fromJson(const QJsonObject& json)
{
    TestStep step;
    step.id = json["id"].toString();
    if (step.id.isEmpty()) {
        step.id = generateId();
    }
    step.order = json["order"].toInt();
    step.category = categoryFromString(json["category"].toString());
    step.command = json["command"].toString();
    step.parameters = json["parameters"].toObject().toVariantMap();
    step.description = json["description"].toString();
    step.enabled = json["enabled"].toBool(true);
    step.continueOnFail = json["continueOnFail"].toBool(false);
    return step;
}

//=============================================================================
// TestCase Implementation
//=============================================================================

bool TestCase::isValid() const
{
    return !id.isEmpty() && !name.isEmpty();
}

int TestCase::enabledStepCount() const
{
    int count = 0;
    for (const auto& step : steps) {
        if (step.enabled) ++count;
    }
    return count;
}

QString TestCase::generateId(const QString& prefix)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return QString("%1_%2").arg(prefix, timestamp);
}

QJsonObject TestCase::toJson() const
{
    QJsonObject obj;
    
    // Mandatory fields
    obj["id"] = id;
    obj["name"] = name;
    obj["description"] = description;
    obj["requirementId"] = requirementId;
    obj["jiraTicket"] = jiraTicket;
    
    // Optional fields
    obj["requirementLink"] = requirementLink;
    obj["jiraLink"] = jiraLink;
    
    QJsonArray tagsArray;
    for (const auto& tag : tags) {
        tagsArray.append(tag);
    }
    obj["tags"] = tagsArray;
    
    obj["priority"] = priority;
    obj["author"] = author;
    obj["createdDate"] = createdDate.toString(Qt::ISODate);
    obj["modifiedDate"] = modifiedDate.toString(Qt::ISODate);
    obj["component"] = component;
    obj["feature"] = feature;
    obj["timeoutMs"] = timeoutMs;
    obj["enabled"] = enabled;
    
    // Config
    obj["config"] = QJsonObject::fromVariantMap(config);
    
    // Steps
    QJsonArray stepsArray;
    for (const auto& step : steps) {
        stepsArray.append(step.toJson());
    }
    obj["steps"] = stepsArray;
    
    return obj;
}

TestCase TestCase::fromJson(const QJsonObject& json)
{
    TestCase tc;
    
    // Mandatory fields
    tc.id = json["id"].toString();
    tc.name = json["name"].toString();
    tc.description = json["description"].toString();
    tc.requirementId = json["requirementId"].toString();
    tc.jiraTicket = json["jiraTicket"].toString();
    
    // Optional fields
    tc.requirementLink = json["requirementLink"].toString();
    tc.jiraLink = json["jiraLink"].toString();
    
    QJsonArray tagsArray = json["tags"].toArray();
    for (const auto& tag : tagsArray) {
        tc.tags.append(tag.toString());
    }
    
    tc.priority = json["priority"].toInt(5);
    tc.author = json["author"].toString();
    tc.createdDate = QDateTime::fromString(json["createdDate"].toString(), Qt::ISODate);
    tc.modifiedDate = QDateTime::fromString(json["modifiedDate"].toString(), Qt::ISODate);
    tc.component = json["component"].toString();
    tc.feature = json["feature"].toString();
    tc.timeoutMs = json["timeoutMs"].toInt(60000);
    tc.enabled = json["enabled"].toBool(true);
    
    // Config
    tc.config = json["config"].toObject().toVariantMap();
    
    // Steps
    QJsonArray stepsArray = json["steps"].toArray();
    for (const auto& stepValue : stepsArray) {
        tc.steps.append(TestStep::fromJson(stepValue.toObject()));
    }
    
    return tc;
}

//=============================================================================
// TestResult Implementation
//=============================================================================

QString TestResult::statusToString(TestStatus status)
{
    switch (status) {
        case TestStatus::NotRun: return "NotRun";
        case TestStatus::Running: return "Running";
        case TestStatus::Passed: return "Passed";
        case TestStatus::Failed: return "Failed";
        case TestStatus::Error: return "Error";
        case TestStatus::Skipped: return "Skipped";
    }
    return "Unknown";
}

TestStatus TestResult::statusFromString(const QString& str)
{
    if (str == "NotRun") return TestStatus::NotRun;
    if (str == "Running") return TestStatus::Running;
    if (str == "Passed") return TestStatus::Passed;
    if (str == "Failed") return TestStatus::Failed;
    if (str == "Error") return TestStatus::Error;
    if (str == "Skipped") return TestStatus::Skipped;
    return TestStatus::NotRun;
}

QJsonObject TestResult::toJson() const
{
    QJsonObject obj;
    obj["testCaseId"] = testCaseId;
    obj["testCaseName"] = testCaseName;
    obj["status"] = statusToString(status);
    obj["statusMessage"] = statusMessage;
    obj["startTime"] = startTime.toString(Qt::ISODate);
    obj["endTime"] = endTime.toString(Qt::ISODate);
    obj["durationMs"] = durationMs;
    obj["totalSteps"] = totalSteps;
    obj["passedSteps"] = passedSteps;
    obj["failedSteps"] = failedSteps;
    obj["skippedSteps"] = skippedSteps;
    obj["requirementId"] = requirementId;
    obj["requirementLink"] = requirementLink;
    obj["jiraTicket"] = jiraTicket;
    obj["jiraLink"] = jiraLink;
    obj["logOutput"] = logOutput;
    obj["screenshotPath"] = screenshotPath;
    
    QJsonArray stepsArray;
    for (const auto& step : stepResults) {
        QJsonObject stepObj = step.toJson();
        stepObj["status"] = statusToString(step.status);
        stepObj["resultMessage"] = step.resultMessage;
        stepObj["durationMs"] = step.durationMs;
        stepObj["responseData"] = QJsonObject::fromVariantMap(step.responseData);
        stepsArray.append(stepObj);
    }
    obj["stepResults"] = stepsArray;
    
    return obj;
}

TestResult TestResult::fromJson(const QJsonObject& json)
{
    TestResult result;
    result.testCaseId = json["testCaseId"].toString();
    result.testCaseName = json["testCaseName"].toString();
    result.status = statusFromString(json["status"].toString());
    result.statusMessage = json["statusMessage"].toString();
    result.startTime = QDateTime::fromString(json["startTime"].toString(), Qt::ISODate);
    result.endTime = QDateTime::fromString(json["endTime"].toString(), Qt::ISODate);
    result.durationMs = json["durationMs"].toInteger();
    result.totalSteps = json["totalSteps"].toInt();
    result.passedSteps = json["passedSteps"].toInt();
    result.failedSteps = json["failedSteps"].toInt();
    result.skippedSteps = json["skippedSteps"].toInt();
    result.requirementId = json["requirementId"].toString();
    result.requirementLink = json["requirementLink"].toString();
    result.jiraTicket = json["jiraTicket"].toString();
    result.jiraLink = json["jiraLink"].toString();
    result.logOutput = json["logOutput"].toString();
    result.screenshotPath = json["screenshotPath"].toString();
    
    QJsonArray stepsArray = json["stepResults"].toArray();
    for (const auto& stepValue : stepsArray) {
        QJsonObject stepObj = stepValue.toObject();
        TestStep step = TestStep::fromJson(stepObj);
        step.status = statusFromString(stepObj["status"].toString());
        step.resultMessage = stepObj["resultMessage"].toString();
        step.durationMs = stepObj["durationMs"].toInteger();
        step.responseData = stepObj["responseData"].toObject().toVariantMap();
        result.stepResults.append(step);
    }
    
    return result;
}

//=============================================================================
// TestSession Implementation
//=============================================================================

QJsonObject TestSession::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["startTime"] = startTime.toString(Qt::ISODate);
    obj["endTime"] = endTime.toString(Qt::ISODate);
    obj["durationMs"] = durationMs;
    obj["totalTests"] = totalTests;
    obj["passedTests"] = passedTests;
    obj["failedTests"] = failedTests;
    obj["errorTests"] = errorTests;
    obj["skippedTests"] = skippedTests;
    obj["configuration"] = QJsonObject::fromVariantMap(configuration);
    obj["environment"] = environment;
    
    QJsonArray resultsArray;
    for (const auto& result : results) {
        resultsArray.append(result.toJson());
    }
    obj["results"] = resultsArray;
    
    return obj;
}

TestSession TestSession::fromJson(const QJsonObject& json)
{
    TestSession session;
    session.id = json["id"].toString();
    session.name = json["name"].toString();
    session.startTime = QDateTime::fromString(json["startTime"].toString(), Qt::ISODate);
    session.endTime = QDateTime::fromString(json["endTime"].toString(), Qt::ISODate);
    session.durationMs = json["durationMs"].toInteger();
    session.totalTests = json["totalTests"].toInt();
    session.passedTests = json["passedTests"].toInt();
    session.failedTests = json["failedTests"].toInt();
    session.errorTests = json["errorTests"].toInt();
    session.skippedTests = json["skippedTests"].toInt();
    session.configuration = json["configuration"].toObject().toVariantMap();
    session.environment = json["environment"].toString();
    
    QJsonArray resultsArray = json["results"].toArray();
    for (const auto& resultValue : resultsArray) {
        session.results.append(TestResult::fromJson(resultValue.toObject()));
    }
    
    return session;
}

//=============================================================================
// TestSuite Implementation
//=============================================================================

QJsonObject TestSuite::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["description"] = description;
    obj["component"] = component;
    
    QJsonArray idsArray;
    for (const auto& tcId : testCaseIds) {
        idsArray.append(tcId);
    }
    obj["testCaseIds"] = idsArray;
    
    return obj;
}

TestSuite TestSuite::fromJson(const QJsonObject& json)
{
    TestSuite suite;
    suite.id = json["id"].toString();
    suite.name = json["name"].toString();
    suite.description = json["description"].toString();
    suite.component = json["component"].toString();
    
    QJsonArray idsArray = json["testCaseIds"].toArray();
    for (const auto& idValue : idsArray) {
        suite.testCaseIds.append(idValue.toString());
    }
    
    return suite;
}

} // namespace TestExecutor
