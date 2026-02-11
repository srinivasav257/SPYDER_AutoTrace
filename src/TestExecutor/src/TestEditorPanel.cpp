/**
 * @file TestEditorPanel.cpp
 * @brief Implementation of Test Editor Dialog.
 */

#include "TestEditorPanel.h"
#include "TestRepository.h"
#include "CommandRegistry.h"
#include "HWConfigManager.h"
#include "ThemeManager.h"
#include <DBCManager.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QTabWidget>
#include <QStandardItemModel>

namespace TestExecutor {

//=============================================================================
// ParameterEditorWidget Implementation
//=============================================================================

ParameterEditorWidget::ParameterEditorWidget(const ParameterDef& paramDef, QWidget* parent)
    : QWidget(parent)
    , m_paramDef(paramDef)
{
    createEditor();
}

void ParameterEditorWidget::createEditor()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    switch (m_paramDef.type) {
        case ParameterType::String:
        case ParameterType::HexString:
        case ParameterType::FilePath:
        {
            auto* edit = new QLineEdit(this);
            edit->setText(m_paramDef.defaultValue.toString());
            edit->setPlaceholderText(m_paramDef.description);
            connect(edit, &QLineEdit::textChanged, this, &ParameterEditorWidget::valueChanged);
            m_editor = edit;
            break;
        }

        case ParameterType::Integer:
        case ParameterType::Duration:
        {
            auto* spin = new QSpinBox(this);
            spin->setMinimum(m_paramDef.minValue.toInt());
            spin->setMaximum(m_paramDef.maxValue.isValid() ? m_paramDef.maxValue.toInt() : 999999);
            spin->setValue(m_paramDef.defaultValue.toInt());
            if (!m_paramDef.unit.isEmpty()) {
                spin->setSuffix(" " + m_paramDef.unit);
            }
            connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
                    this, &ParameterEditorWidget::valueChanged);
            m_editor = spin;
            break;
        }

        case ParameterType::CanId:
        {
            // Editable combo box populated with DBC messages from all channels
            auto* combo = new QComboBox(this);
            combo->setEditable(true);
            combo->setInsertPolicy(QComboBox::NoInsert);
            combo->setMinimumWidth(200);

            // Populate from all loaded DBC databases (both CAN channels)
            auto& dbcMgr = DBCManager::DBCDatabaseManager::instance();
            for (int ch = 0; ch < DBCManager::DBCDatabaseManager::MAX_CHANNELS; ++ch) {
                if (!dbcMgr.isLoaded(ch))
                    continue;
                QStringList msgList = dbcMgr.messageDisplayList(ch);
                if (!msgList.isEmpty()) {
                    QString header = QString("--- CAN %1 ---").arg(ch + 1);
                    combo->addItem(header);
                    // Make header non-selectable
                    auto* model = qobject_cast<QStandardItemModel*>(combo->model());
                    if (model) {
                        auto* item = model->item(combo->count() - 1);
                        if (item) {
                            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                            item->setData(QBrush(Qt::gray), Qt::ForegroundRole);
                        }
                    }
                    for (const QString& msgDisplay : msgList) {
                        // Store the CAN ID as user data for easy retrieval
                        uint32_t canId = dbcMgr.resolveMessageId(ch, msgDisplay);
                        combo->addItem(msgDisplay, canId);
                    }
                }
            }

            // Set default or restore value
            QString defaultVal = m_paramDef.defaultValue.toString();
            if (!defaultVal.isEmpty()) {
                int idx = combo->findText(defaultVal, Qt::MatchContains);
                if (idx >= 0)
                    combo->setCurrentIndex(idx);
                else
                    combo->setEditText(defaultVal);
            }

            // Auto-refresh when DBC databases change
            connect(&dbcMgr, &DBCManager::DBCDatabaseManager::messageListChanged,
                    this, [combo, this](int /*channelIndex*/) {
                // Preserve current text
                QString currentText = combo->currentText();
                combo->clear();
                auto& mgr = DBCManager::DBCDatabaseManager::instance();
                for (int ch = 0; ch < DBCManager::DBCDatabaseManager::MAX_CHANNELS; ++ch) {
                    if (!mgr.isLoaded(ch))
                        continue;
                    QStringList msgList = mgr.messageDisplayList(ch);
                    if (!msgList.isEmpty()) {
                        combo->addItem(QString("--- CAN %1 ---").arg(ch + 1));
                        auto* model = qobject_cast<QStandardItemModel*>(combo->model());
                        if (model) {
                            auto* item = model->item(combo->count() - 1);
                            if (item)
                                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                        }
                        for (const QString& msgDisplay : msgList) {
                            uint32_t canId = mgr.resolveMessageId(ch, msgDisplay);
                            combo->addItem(msgDisplay, canId);
                        }
                    }
                }
                // Restore previous selection
                if (!currentText.isEmpty()) {
                    int idx = combo->findText(currentText, Qt::MatchContains);
                    if (idx >= 0)
                        combo->setCurrentIndex(idx);
                    else
                        combo->setEditText(currentText);
                }
            });

            connect(combo, &QComboBox::currentTextChanged,
                    this, &ParameterEditorWidget::valueChanged);
            m_editor = combo;
            break;
        }

        case ParameterType::Double:
        case ParameterType::Voltage:
        case ParameterType::Current:
        {
            auto* spin = new QDoubleSpinBox(this);
            spin->setMinimum(m_paramDef.minValue.toDouble());
            spin->setMaximum(m_paramDef.maxValue.isValid() ? m_paramDef.maxValue.toDouble() : 9999.99);
            spin->setValue(m_paramDef.defaultValue.toDouble());
            spin->setDecimals(2);
            if (!m_paramDef.unit.isEmpty()) {
                spin->setSuffix(" " + m_paramDef.unit);
            }
            connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    this, &ParameterEditorWidget::valueChanged);
            m_editor = spin;
            break;
        }

        case ParameterType::Boolean:
        {
            auto* check = new QCheckBox(m_paramDef.displayName, this);
            check->setChecked(m_paramDef.defaultValue.toBool());
            connect(check, &QCheckBox::toggled, this, &ParameterEditorWidget::valueChanged);
            m_editor = check;
            break;
        }

        case ParameterType::Enum:
        {
            auto* combo = new QComboBox(this);
            combo->addItems(m_paramDef.enumValues);
            combo->setCurrentText(m_paramDef.defaultValue.toString());
            connect(combo, &QComboBox::currentTextChanged, this, &ParameterEditorWidget::valueChanged);
            m_editor = combo;
            break;
        }

        case ParameterType::ComPort:
        {
            auto* combo = new QComboBox(this);
            combo->setEditable(true);
            // Populate with configured port aliases from HW Config
            const auto aliases = HWConfigManager::instance().allPortAliases();
            combo->addItems(aliases);
            // Set current value
            QString defaultVal = m_paramDef.defaultValue.toString();
            if (!defaultVal.isEmpty()) {
                int idx = combo->findText(defaultVal, Qt::MatchContains);
                if (idx >= 0)
                    combo->setCurrentIndex(idx);
                else
                    combo->setEditText(defaultVal);
            }
            connect(combo, &QComboBox::currentTextChanged, this, &ParameterEditorWidget::valueChanged);
            m_editor = combo;
            break;
        }

        default:
        {
            auto* edit = new QLineEdit(this);
            edit->setText(m_paramDef.defaultValue.toString());
            m_editor = edit;
            break;
        }
    }

    layout->addWidget(m_editor);
}

QVariant ParameterEditorWidget::value() const
{
    if (auto* edit = qobject_cast<QLineEdit*>(m_editor)) {
        return edit->text();
    }
    if (auto* spin = qobject_cast<QSpinBox*>(m_editor)) {
        return spin->value();
    }
    if (auto* spin = qobject_cast<QDoubleSpinBox*>(m_editor)) {
        return spin->value();
    }
    if (auto* check = qobject_cast<QCheckBox*>(m_editor)) {
        return check->isChecked();
    }
    if (auto* combo = qobject_cast<QComboBox*>(m_editor)) {
        return combo->currentText();
    }
    return QVariant();
}

void ParameterEditorWidget::setValue(const QVariant& value)
{
    if (auto* edit = qobject_cast<QLineEdit*>(m_editor)) {
        edit->setText(value.toString());
        return;
    }
    if (auto* spin = qobject_cast<QSpinBox*>(m_editor)) {
        spin->setValue(value.toInt());
        return;
    }
    if (auto* spin = qobject_cast<QDoubleSpinBox*>(m_editor)) {
        spin->setValue(value.toDouble());
        return;
    }
    if (auto* check = qobject_cast<QCheckBox*>(m_editor)) {
        check->setChecked(value.toBool());
        return;
    }
    if (auto* combo = qobject_cast<QComboBox*>(m_editor)) {
        combo->setCurrentText(value.toString());
        return;
    }
}

//=============================================================================
// StepEditorWidget Implementation
//=============================================================================

StepEditorWidget::StepEditorWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Placeholder shown when no step is selected
    m_placeholderLabel = new QLabel(tr("Select a step to configure"), this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_placeholderLabel);

    // Form container (hidden by default)
    m_formWidget = new QWidget(this);
    auto* formWidgetLayout = new QVBoxLayout(m_formWidget);
    formWidgetLayout->setContentsMargins(0, 0, 0, 0);

    auto* formLayout = new QFormLayout();

    // Category selector
    m_categoryCombo = new QComboBox(m_formWidget);
    m_categoryCombo->addItems({"Serial", "CAN", "Power", "Flow", "Validation", "System",
                               "ManDiagITS", "ManDiagPITS", "ManDiagMOL"});
    formLayout->addRow("Category:", m_categoryCombo);

    // Command selector
    m_commandCombo = new QComboBox(m_formWidget);
    formLayout->addRow("Command:", m_commandCombo);

    // Description
    m_descriptionEdit = new QLineEdit(m_formWidget);
    formLayout->addRow("Description:", m_descriptionEdit);

    // Enabled checkbox
    m_enabledCheck = new QCheckBox("Enabled", m_formWidget);
    m_enabledCheck->setChecked(true);

    m_continueOnFailCheck = new QCheckBox("Continue on Failure", m_formWidget);

    auto* checkLayout = new QHBoxLayout();
    checkLayout->addWidget(m_enabledCheck);
    checkLayout->addWidget(m_continueOnFailCheck);
    checkLayout->addStretch();
    formLayout->addRow("Options:", checkLayout);

    formWidgetLayout->addLayout(formLayout);

    // Parameters container
    auto* paramGroup = new QGroupBox("Parameters", m_formWidget);
    m_parameterLayout = new QFormLayout(paramGroup);
    m_parameterContainer = paramGroup;
    formWidgetLayout->addWidget(paramGroup);

    formWidgetLayout->addStretch();

    layout->addWidget(m_formWidget);
    m_formWidget->hide();

    // Connect signals
    connect(m_categoryCombo, &QComboBox::currentTextChanged, this, &StepEditorWidget::onCategoryChanged);
    connect(m_commandCombo, &QComboBox::currentTextChanged, this, &StepEditorWidget::onCommandChanged);
    connect(m_descriptionEdit, &QLineEdit::textChanged, this, &StepEditorWidget::stepModified);
    connect(m_enabledCheck, &QCheckBox::toggled, this, &StepEditorWidget::stepModified);
    connect(m_continueOnFailCheck, &QCheckBox::toggled, this, &StepEditorWidget::stepModified);
}

void StepEditorWidget::loadStep(const TestStep& step)
{
    m_currentStep = step;

    // Show the form, hide placeholder
    m_placeholderLabel->hide();
    m_formWidget->show();

    // Block signals during load
    m_categoryCombo->blockSignals(true);
    m_commandCombo->blockSignals(true);
    m_descriptionEdit->blockSignals(true);
    m_enabledCheck->blockSignals(true);
    m_continueOnFailCheck->blockSignals(true);

    // Set category
    m_categoryCombo->setCurrentText(TestStep::categoryToString(step.category));

    // Populate command combo for this category without triggering onCommandChanged
    {
        CommandCategory cat = step.category;
        auto& registry = CommandRegistry::instance();
        m_commandCombo->clear();
        auto commands = registry.commandsByCategory(cat);
        for (const auto& cmd : commands) {
            m_commandCombo->addItem(cmd.name, cmd.id);
        }
    }

    // Find and set command by id
    auto& registry = CommandRegistry::instance();
    const CommandDef* cmd = registry.command(step.command);
    if (cmd) {
        m_commandCombo->setCurrentText(cmd->name);
    }

    m_descriptionEdit->setText(step.description);
    m_enabledCheck->setChecked(step.enabled);
    m_continueOnFailCheck->setChecked(step.continueOnFail);

    // Build parameter editors for the correct command
    updateParameterEditors();

    // Load parameter values
    for (ParameterEditorWidget* editor : m_parameterEditors) {
        QString paramName = editor->parameterDef().name;
        if (step.parameters.contains(paramName)) {
            editor->setValue(step.parameters.value(paramName));
        }
    }

    m_categoryCombo->blockSignals(false);
    m_commandCombo->blockSignals(false);
    m_descriptionEdit->blockSignals(false);
    m_enabledCheck->blockSignals(false);
    m_continueOnFailCheck->blockSignals(false);
}

TestStep StepEditorWidget::getStep() const
{
    TestStep step = m_currentStep;

    step.category = TestStep::categoryFromString(m_categoryCombo->currentText());

    // Get command id from selected name
    auto& registry = CommandRegistry::instance();
    auto commands = registry.commandsByCategory(step.category);
    for (const auto& cmd : commands) {
        if (cmd.name == m_commandCombo->currentText()) {
            step.command = cmd.id;
            break;
        }
    }

    step.description = m_descriptionEdit->text();
    step.enabled = m_enabledCheck->isChecked();
    step.continueOnFail = m_continueOnFailCheck->isChecked();

    // Gather parameters
    step.parameters.clear();
    for (ParameterEditorWidget* editor : m_parameterEditors) {
        step.parameters[editor->parameterDef().name] = editor->value();
    }

    return step;
}

void StepEditorWidget::clear()
{
    m_currentStep = TestStep();
    m_categoryCombo->setCurrentIndex(0);
    m_commandCombo->clear();
    m_descriptionEdit->clear();
    m_enabledCheck->setChecked(true);
    m_continueOnFailCheck->setChecked(false);
    clearParameterEditors();

    // Show placeholder, hide form
    m_formWidget->hide();
    m_placeholderLabel->show();
}

void StepEditorWidget::setCommand(const QString& commandId)
{
    auto& registry = CommandRegistry::instance();
    const CommandDef* cmd = registry.command(commandId);
    if (cmd) {
        m_categoryCombo->setCurrentText(TestStep::categoryToString(cmd->category));
        m_commandCombo->setCurrentText(cmd->name);
    }
}

void StepEditorWidget::onCategoryChanged(const QString& category)
{
    CommandCategory cat = TestStep::categoryFromString(category);
    auto& registry = CommandRegistry::instance();

    m_commandCombo->clear();

    auto commands = registry.commandsByCategory(cat);
    for (const auto& cmd : commands) {
        m_commandCombo->addItem(cmd.name, cmd.id);
    }

    if (m_commandCombo->count() > 0) {
        onCommandChanged(m_commandCombo->currentText());
    }
}

void StepEditorWidget::onCommandChanged(const QString& command)
{
    // Get command id from combo data
    int index = m_commandCombo->findText(command);
    if (index < 0) return;

    QString commandId = m_commandCombo->itemData(index).toString();
    if (commandId.isEmpty()) {
        // Fallback: find by name
        auto& registry = CommandRegistry::instance();
        CommandCategory cat = TestStep::categoryFromString(m_categoryCombo->currentText());
        auto commands = registry.commandsByCategory(cat);
        for (const auto& cmd : commands) {
            if (cmd.name == command) {
                commandId = cmd.id;
                break;
            }
        }
    }

    m_currentStep.command = commandId;
    updateParameterEditors();
    emit stepModified();
}

void StepEditorWidget::updateParameterEditors()
{
    clearParameterEditors();

    auto& registry = CommandRegistry::instance();
    const CommandDef* cmd = registry.command(m_currentStep.command);
    if (!cmd) return;

    for (const ParameterDef& paramDef : cmd->parameters) {
        auto* editor = new ParameterEditorWidget(paramDef, this);

        // Set existing value if available
        if (m_currentStep.parameters.contains(paramDef.name)) {
            editor->setValue(m_currentStep.parameters.value(paramDef.name));
        }

        connect(editor, &ParameterEditorWidget::valueChanged, this, &StepEditorWidget::stepModified);

        QString label = paramDef.displayName;
        if (paramDef.required) {
            label += " *";
        }
        m_parameterLayout->addRow(label + ":", editor);
        m_parameterEditors.append(editor);
    }
}

void StepEditorWidget::clearParameterEditors()
{
    for (ParameterEditorWidget* editor : m_parameterEditors) {
        m_parameterLayout->removeWidget(editor);
        delete editor;
    }
    m_parameterEditors.clear();

    // Clear any remaining rows
    while (m_parameterLayout->rowCount() > 0) {
        m_parameterLayout->removeRow(0);
    }
}

//=============================================================================
// TestEditorDialog Implementation
//=============================================================================

TestEditorDialog::TestEditorDialog(QWidget* parent)
    : QDialog(parent)
{
    setObjectName(QStringLiteral("testEditorDialog"));
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    StyleLib::ThemeManager::instance().applyScopedStyle(this, StyleLib::ScopedStyle::TestExecutor);
    StyleLib::ThemeManager::setDarkTitleBar(this);
    setupConnections();
}

TestEditorDialog::~TestEditorDialog() = default;

void TestEditorDialog::loadTestCase(const QString& testCaseId)
{
    if (m_dirty) {
        int result = QMessageBox::question(
            this, "Unsaved Changes",
            "You have unsaved changes. Do you want to save them?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Save) {
            save();
        } else if (result == QMessageBox::Cancel) {
            return;
        }
    }

    const TestCase* tc = TestRepository::instance().testCase(testCaseId);
    if (!tc) {
        clear();
        return;
    }

    m_currentTestCaseId = testCaseId;
    m_editingTestCase = *tc;

    // Update dialog title
    setWindowTitle(QString("Test Editor - %1").arg(tc->name));

    // Populate metadata
    m_nameEdit->setText(tc->name);
    m_descriptionEdit->setText(tc->description);
    m_requirementIdEdit->setText(tc->requirementId);
    m_requirementLinkEdit->setText(tc->requirementLink);
    m_jiraTicketEdit->setText(tc->jiraTicket);
    m_jiraLinkEdit->setText(tc->jiraLink);
    m_componentEdit->setText(tc->component);
    m_featureEdit->setText(tc->feature);
    m_tagsEdit->setText(tc->tags.join(", "));
    m_prioritySpin->setValue(tc->priority);
    m_timeoutSpin->setValue(tc->timeoutMs);
    m_enabledCheck->setChecked(tc->enabled);

    // Populate steps table
    populateStepsTable();

    setDirty(false);
}

bool TestEditorDialog::save()
{
    if (m_currentTestCaseId.isEmpty()) {
        return false;
    }

    // Update test case from UI
    m_editingTestCase.name = m_nameEdit->text();
    m_editingTestCase.description = m_descriptionEdit->text();
    m_editingTestCase.requirementId = m_requirementIdEdit->text();
    m_editingTestCase.requirementLink = m_requirementLinkEdit->text();
    m_editingTestCase.jiraTicket = m_jiraTicketEdit->text();
    m_editingTestCase.jiraLink = m_jiraLinkEdit->text();
    m_editingTestCase.component = m_componentEdit->text();
    m_editingTestCase.feature = m_featureEdit->text();
    m_editingTestCase.tags = m_tagsEdit->text().split(",", Qt::SkipEmptyParts);
    for (QString& tag : m_editingTestCase.tags) {
        tag = tag.trimmed();
    }
    m_editingTestCase.priority = m_prioritySpin->value();
    m_editingTestCase.timeoutMs = m_timeoutSpin->value();
    m_editingTestCase.enabled = m_enabledCheck->isChecked();

    // Save to repository
    if (TestRepository::instance().updateTestCase(m_editingTestCase)) {
        setDirty(false);
        setWindowTitle(QString("Test Editor - %1").arg(m_editingTestCase.name));
        emit testCaseSaved(m_currentTestCaseId);
        return true;
    }

    return false;
}

void TestEditorDialog::revert()
{
    if (!m_currentTestCaseId.isEmpty()) {
        loadTestCase(m_currentTestCaseId);
    }
}

void TestEditorDialog::clear()
{
    m_currentTestCaseId.clear();
    m_editingTestCase = TestCase();

    m_nameEdit->clear();
    m_descriptionEdit->clear();
    m_requirementIdEdit->clear();
    m_requirementLinkEdit->clear();
    m_jiraTicketEdit->clear();
    m_jiraLinkEdit->clear();
    m_componentEdit->clear();
    m_featureEdit->clear();
    m_tagsEdit->clear();
    m_prioritySpin->setValue(5);
    m_timeoutSpin->setValue(60000);
    m_enabledCheck->setChecked(true);

    m_stepsTable->setRowCount(0);
    m_stepEditor->clear();

    setDirty(false);
    setWindowTitle("Test Editor");
}

void TestEditorDialog::setupUi()
{
    setWindowTitle("Test Editor");
    resize(900, 700);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    // === Tab Widget ===
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    // =====================================================================
    // TAB 1: Test Case Information
    // =====================================================================
    auto* infoTab = new QWidget();
    auto* infoLayout = new QVBoxLayout(infoTab);
    infoLayout->setContentsMargins(8, 12, 8, 8);

    auto* metaLayout = new QFormLayout();
    metaLayout->setSpacing(6);

    m_nameEdit = new QLineEdit(infoTab);
    m_nameEdit->setPlaceholderText("Enter test name...");
    metaLayout->addRow("Name *:", m_nameEdit);

    m_descriptionEdit = new QLineEdit(infoTab);
    m_descriptionEdit->setPlaceholderText("Enter description...");
    metaLayout->addRow("Description *:", m_descriptionEdit);

    m_requirementIdEdit = new QLineEdit(infoTab);
    m_requirementIdEdit->setPlaceholderText("REQ-XXX-XXX");
    metaLayout->addRow("Requirement ID *:", m_requirementIdEdit);

    m_requirementLinkEdit = new QLineEdit(infoTab);
    m_requirementLinkEdit->setPlaceholderText("https://polarion.example.com/...");
    metaLayout->addRow("Requirement Link:", m_requirementLinkEdit);

    m_jiraTicketEdit = new QLineEdit(infoTab);
    m_jiraTicketEdit->setPlaceholderText("PROJ-1234");
    metaLayout->addRow("JIRA Ticket *:", m_jiraTicketEdit);

    m_jiraLinkEdit = new QLineEdit(infoTab);
    m_jiraLinkEdit->setPlaceholderText("https://jira.example.com/browse/...");
    metaLayout->addRow("JIRA Link:", m_jiraLinkEdit);

    m_componentEdit = new QLineEdit(infoTab);
    metaLayout->addRow("Group:", m_componentEdit);

    m_featureEdit = new QLineEdit(infoTab);
    metaLayout->addRow("Feature:", m_featureEdit);

    m_tagsEdit = new QLineEdit(infoTab);
    m_tagsEdit->setPlaceholderText("EOL, ManDiag, Smoke");
    metaLayout->addRow("Tags:", m_tagsEdit);

    auto* optionsLayout = new QHBoxLayout();
    m_prioritySpin = new QSpinBox(infoTab);
    m_prioritySpin->setRange(1, 10);
    m_prioritySpin->setValue(5);
    optionsLayout->addWidget(new QLabel("Priority:", infoTab));
    optionsLayout->addWidget(m_prioritySpin);

    m_timeoutSpin = new QSpinBox(infoTab);
    m_timeoutSpin->setRange(1000, 600000);
    m_timeoutSpin->setValue(60000);
    m_timeoutSpin->setSuffix(" ms");
    optionsLayout->addWidget(new QLabel("Timeout:", infoTab));
    optionsLayout->addWidget(m_timeoutSpin);

    m_enabledCheck = new QCheckBox("Enabled", infoTab);
    m_enabledCheck->setChecked(true);
    optionsLayout->addWidget(m_enabledCheck);
    optionsLayout->addStretch();

    metaLayout->addRow("Options:", optionsLayout);

    infoLayout->addLayout(metaLayout);
    infoLayout->addStretch();

    m_tabWidget->addTab(infoTab, "Test Case Info");

    // =====================================================================
    // TAB 2: Test Steps & Step Configuration
    // =====================================================================
    auto* stepsTab = new QWidget();
    auto* stepsTabLayout = new QVBoxLayout(stepsTab);
    stepsTabLayout->setContentsMargins(8, 12, 8, 8);

    auto* splitter = new QSplitter(Qt::Vertical, stepsTab);

    // --- Steps Table Section ---
    auto* stepsGroup = new QGroupBox("Test Steps", stepsTab);
    auto* stepsLayout = new QVBoxLayout(stepsGroup);
    stepsLayout->setContentsMargins(8, 12, 8, 8);

    // Steps toolbar
    auto* stepsToolbar = new QHBoxLayout();
    m_btnAddStep = new QPushButton("+ Add Step", stepsGroup);
    m_btnRemoveStep = new QPushButton("- Remove", stepsGroup);
    m_btnMoveUp = new QPushButton("Up", stepsGroup);
    m_btnMoveDown = new QPushButton("Down", stepsGroup);
    m_btnDuplicate = new QPushButton("Duplicate", stepsGroup);

    stepsToolbar->addWidget(m_btnAddStep);
    stepsToolbar->addWidget(m_btnRemoveStep);
    stepsToolbar->addWidget(m_btnMoveUp);
    stepsToolbar->addWidget(m_btnMoveDown);
    stepsToolbar->addWidget(m_btnDuplicate);
    stepsToolbar->addStretch();
    stepsLayout->addLayout(stepsToolbar);

    // Steps table
    m_stepsTable = new QTableWidget(stepsGroup);
    m_stepsTable->setColumnCount(5);
    m_stepsTable->setHorizontalHeaderLabels({"#", "Category", "Command", "Parameters", "Status"});
    m_stepsTable->horizontalHeader()->setStretchLastSection(true);
    m_stepsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_stepsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_stepsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_stepsTable->verticalHeader()->setVisible(false);
    m_stepsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_stepsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_stepsTable->setAlternatingRowColors(true);
    stepsLayout->addWidget(m_stepsTable);

    splitter->addWidget(stepsGroup);

    // --- Step Configuration Section ---
    auto* stepEditorGroup = new QGroupBox("Step Configuration", stepsTab);
    auto* stepEditorLayout = new QVBoxLayout(stepEditorGroup);
    stepEditorLayout->setContentsMargins(8, 12, 8, 8);

    auto* scrollArea = new QScrollArea(stepEditorGroup);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    m_stepEditor = new StepEditorWidget(scrollArea);
    scrollArea->setWidget(m_stepEditor);
    stepEditorLayout->addWidget(scrollArea);

    splitter->addWidget(stepEditorGroup);

    // Set splitter proportions
    splitter->setStretchFactor(0, 1);  // Steps: expands
    splitter->setStretchFactor(1, 1);  // Step Config: expands equally
    splitter->setSizes({350, 300});

    stepsTabLayout->addWidget(splitter);

    m_tabWidget->addTab(stepsTab, "Test Steps");

    // =====================================================================
    // Bottom Buttons (Save / Revert)
    // =====================================================================
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_btnRevert = new QPushButton("Revert", this);
    m_btnSave = new QPushButton("Save", this);

    buttonLayout->addWidget(m_btnRevert);
    buttonLayout->addWidget(m_btnSave);
    mainLayout->addLayout(buttonLayout);

    m_statusBar = new QStatusBar(this);
    m_statusBar->setSizeGripEnabled(false);
    mainLayout->addWidget(m_statusBar);

    m_btnRevert->setEnabled(false);
}

void TestEditorDialog::setupConnections()
{
    // Metadata changes
    connect(m_nameEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_descriptionEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_requirementIdEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_requirementLinkEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_jiraTicketEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_jiraLinkEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_componentEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_featureEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_tagsEdit, &QLineEdit::textChanged, this, &TestEditorDialog::onMetadataChanged);
    connect(m_prioritySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TestEditorDialog::onMetadataChanged);
    connect(m_timeoutSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TestEditorDialog::onMetadataChanged);
    connect(m_enabledCheck, &QCheckBox::toggled, this, &TestEditorDialog::onMetadataChanged);

    // Step actions
    connect(m_btnAddStep, &QPushButton::clicked, this, &TestEditorDialog::onAddStepClicked);
    connect(m_btnRemoveStep, &QPushButton::clicked, this, &TestEditorDialog::onRemoveStepClicked);
    connect(m_btnMoveUp, &QPushButton::clicked, this, &TestEditorDialog::onMoveStepUpClicked);
    connect(m_btnMoveDown, &QPushButton::clicked, this, &TestEditorDialog::onMoveStepDownClicked);
    connect(m_btnDuplicate, &QPushButton::clicked, this, &TestEditorDialog::onDuplicateStepClicked);

    // Table selection
    connect(m_stepsTable, &QTableWidget::itemSelectionChanged, this, &TestEditorDialog::onStepSelectionChanged);

    // Step editor
    connect(m_stepEditor, &StepEditorWidget::stepModified, this, [this]() {
        if (m_selectedStepRow >= 0) {
            syncStepFromEditor(m_selectedStepRow);
            setDirty(true);
        }
    });

    // Save/Revert
    connect(m_btnSave, &QPushButton::clicked, this, &TestEditorDialog::onSaveClicked);
    connect(m_btnRevert, &QPushButton::clicked, this, &TestEditorDialog::onRevertClicked);
}

void TestEditorDialog::populateStepsTable()
{
    m_stepsTable->setRowCount(m_editingTestCase.steps.size());

    for (int i = 0; i < m_editingTestCase.steps.size(); ++i) {
        const TestStep& step = m_editingTestCase.steps[i];

        m_stepsTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_stepsTable->setItem(i, 1, new QTableWidgetItem(TestStep::categoryToString(step.category)));

        // Get command name
        const CommandDef* cmd = CommandRegistry::instance().command(step.command);
        QString cmdName = cmd ? cmd->name : step.command;
        m_stepsTable->setItem(i, 2, new QTableWidgetItem(cmdName));

        // Format parameters
        QStringList paramStrs;
        for (auto it = step.parameters.constBegin(); it != step.parameters.constEnd(); ++it) {
            paramStrs.append(QString("%1=%2").arg(it.key(), it.value().toString()));
        }
        m_stepsTable->setItem(i, 3, new QTableWidgetItem(paramStrs.join(", ")));

        // Status
        QString status = step.enabled ? "Enabled" : "Disabled";
        m_stepsTable->setItem(i, 4, new QTableWidgetItem(status));
    }
}

void TestEditorDialog::updateStepEditor(int row)
{
    if (row < 0 || row >= m_editingTestCase.steps.size()) {
        m_stepEditor->clear();
        return;
    }

    m_stepEditor->loadStep(m_editingTestCase.steps[row]);
}

void TestEditorDialog::syncStepFromEditor(int row)
{
    if (row < 0 || row >= m_editingTestCase.steps.size()) {
        return;
    }

    TestStep updatedStep = m_stepEditor->getStep();
    updatedStep.id = m_editingTestCase.steps[row].id; // Preserve ID
    updatedStep.order = row + 1;
    m_editingTestCase.steps[row] = updatedStep;

    // Update table row
    const CommandDef* cmd = CommandRegistry::instance().command(updatedStep.command);
    QString cmdName = cmd ? cmd->name : updatedStep.command;

    if (row >= m_stepsTable->rowCount()) {
        return;
    }

    auto setItemText = [this](int r, int col, const QString& text) {
        QTableWidgetItem* item = m_stepsTable->item(r, col);
        if (item) {
            item->setText(text);
        }
    };

    setItemText(row, 1, TestStep::categoryToString(updatedStep.category));
    setItemText(row, 2, cmdName);

    QStringList paramStrs;
    for (auto it = updatedStep.parameters.constBegin(); it != updatedStep.parameters.constEnd(); ++it) {
        paramStrs.append(QString("%1=%2").arg(it.key(), it.value().toString()));
    }
    setItemText(row, 3, paramStrs.join(", "));
    setItemText(row, 4, updatedStep.enabled ? "Enabled" : "Disabled");
}

void TestEditorDialog::setDirty(bool dirty)
{
    if (m_dirty != dirty) {
        m_dirty = dirty;
        emit dirtyStateChanged(dirty);

        m_btnSave->setEnabled(true);
        m_btnRevert->setEnabled(dirty);
    }
}

void TestEditorDialog::onMetadataChanged()
{
    setDirty(true);
}

void TestEditorDialog::onAddStepClicked()
{
    TestStep newStep;
    newStep.id = TestStep::generateId();
    newStep.order = m_editingTestCase.steps.size() + 1;
    newStep.category = CommandCategory::Serial;
    newStep.command = "serial_enter_md_session";
    newStep.description = "New step";
    newStep.enabled = true;

    m_editingTestCase.steps.append(newStep);
    populateStepsTable();

    // Select the new row
    m_stepsTable->selectRow(m_editingTestCase.steps.size() - 1);
    setDirty(true);
}

void TestEditorDialog::onRemoveStepClicked()
{
    int row = m_stepsTable->currentRow();
    if (row < 0 || row >= m_editingTestCase.steps.size()) {
        return;
    }

    m_editingTestCase.steps.removeAt(row);
    populateStepsTable();
    setDirty(true);
}

void TestEditorDialog::onMoveStepUpClicked()
{
    int row = m_stepsTable->currentRow();
    if (row <= 0) return;

    m_editingTestCase.steps.swapItemsAt(row, row - 1);
    populateStepsTable();
    m_stepsTable->selectRow(row - 1);
    setDirty(true);
}

void TestEditorDialog::onMoveStepDownClicked()
{
    int row = m_stepsTable->currentRow();
    if (row < 0 || row >= m_editingTestCase.steps.size() - 1) return;

    m_editingTestCase.steps.swapItemsAt(row, row + 1);
    populateStepsTable();
    m_stepsTable->selectRow(row + 1);
    setDirty(true);
}

void TestEditorDialog::onDuplicateStepClicked()
{
    int row = m_stepsTable->currentRow();
    if (row < 0 || row >= m_editingTestCase.steps.size()) return;

    TestStep copy = m_editingTestCase.steps[row];
    copy.id = TestStep::generateId();
    copy.description += " (Copy)";

    m_editingTestCase.steps.insert(row + 1, copy);
    populateStepsTable();
    m_stepsTable->selectRow(row + 1);
    setDirty(true);
}

void TestEditorDialog::onStepSelectionChanged()
{
    // Sync previous step if any
    if (m_selectedStepRow >= 0 && m_selectedStepRow < m_editingTestCase.steps.size()) {
        syncStepFromEditor(m_selectedStepRow);
    }

    m_selectedStepRow = m_stepsTable->currentRow();
    updateStepEditor(m_selectedStepRow);
}

void TestEditorDialog::onStepTableCellChanged(int row, int /*column*/)
{
    Q_UNUSED(row)
    setDirty(true);
}

void TestEditorDialog::onSaveClicked()
{
    if (save()) {
        if (m_statusBar) {
            m_statusBar->showMessage("Test case saved successfully.", 3000);
        }
    } else {
        if (m_statusBar) {
            m_statusBar->showMessage("Save failed. Unable to update test case.", 5000);
        }
    }
}

void TestEditorDialog::onRevertClicked()
{
    if (m_dirty) {
        int result = QMessageBox::question(
            this, "Revert Changes",
            "Are you sure you want to discard all changes?",
            QMessageBox::Yes | QMessageBox::No);

        if (result == QMessageBox::Yes) {
            revert();
        }
    }
}

} // namespace TestExecutor
