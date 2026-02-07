#pragma once
/**
 * @file TestEditorPanel.h
 * @brief Test Editor dialog for editing test cases and steps.
 *
 * Features:
 * - Edit test case metadata (name, description, requirement, JIRA)
 * - Add/Remove/Reorder test steps
 * - Command selection from predefined commands (no coding!)
 * - Parameter editing with type-specific editors
 * - Validation and preview
 * - Tabbed layout: Tab 1 = Test Case Info, Tab 2 = Test Steps & Configuration
 */

#include "TestDataModels.h"
#include "CommandRegistry.h"
#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QStackedWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>

namespace TestExecutor {

//=============================================================================
// ParameterEditorWidget - Dynamic parameter editing
//=============================================================================

/**
 * @brief Widget for editing a single parameter based on its type.
 */
class ParameterEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterEditorWidget(const ParameterDef& paramDef, QWidget* parent = nullptr);

    /**
     * @brief Get the current value
     */
    QVariant value() const;

    /**
     * @brief Set the value
     */
    void setValue(const QVariant& value);

    /**
     * @brief Get the parameter definition
     */
    const ParameterDef& parameterDef() const { return m_paramDef; }

signals:
    void valueChanged();

private:
    void createEditor();

    ParameterDef m_paramDef;
    QWidget* m_editor = nullptr;
};

//=============================================================================
// StepEditorWidget - Edit a single step's parameters
//=============================================================================

/**
 * @brief Widget for editing all parameters of a test step.
 */
class StepEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StepEditorWidget(QWidget* parent = nullptr);

    /**
     * @brief Load a step for editing
     */
    void loadStep(const TestStep& step);

    /**
     * @brief Get the edited step
     */
    TestStep getStep() const;

    /**
     * @brief Clear the editor
     */
    void clear();

    /**
     * @brief Set command by ID (updates parameters UI)
     */
    void setCommand(const QString& commandId);

signals:
    void stepModified();

private slots:
    void onCategoryChanged(const QString& category);
    void onCommandChanged(const QString& command);

private:
    void updateParameterEditors();
    void clearParameterEditors();

    QComboBox* m_categoryCombo = nullptr;
    QComboBox* m_commandCombo = nullptr;
    QLineEdit* m_descriptionEdit = nullptr;
    QCheckBox* m_enabledCheck = nullptr;
    QCheckBox* m_continueOnFailCheck = nullptr;
    QWidget* m_parameterContainer = nullptr;
    QFormLayout* m_parameterLayout = nullptr;
    QVector<ParameterEditorWidget*> m_parameterEditors;

    QLabel* m_placeholderLabel = nullptr;
    QWidget* m_formWidget = nullptr;

    TestStep m_currentStep;
};

//=============================================================================
// TestEditorDialog - Main editor dialog (opened on test double-click)
//=============================================================================

/**
 * @brief Dialog for editing test cases and their steps.
 *
 * Opened when user double-clicks a test case in the Test Explorer.
 * Tab 1: Test Case Information (metadata)
 * Tab 2: Test Steps and Step Configuration
 */
class TestEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestEditorDialog(QWidget* parent = nullptr);
    ~TestEditorDialog() override;

    /**
     * @brief Load a test case for editing
     */
    void loadTestCase(const QString& testCaseId);

    /**
     * @brief Get the current test case ID
     */
    QString currentTestCaseId() const { return m_currentTestCaseId; }

    /**
     * @brief Check if there are unsaved changes
     */
    bool hasUnsavedChanges() const { return m_dirty; }

    /**
     * @brief Save changes to repository
     */
    bool save();

    /**
     * @brief Discard changes and reload
     */
    void revert();

    /**
     * @brief Clear the editor
     */
    void clear();

signals:
    /**
     * @brief Emitted when test case is saved
     */
    void testCaseSaved(const QString& testCaseId);

    /**
     * @brief Emitted when dirty state changes
     */
    void dirtyStateChanged(bool dirty);

private slots:
    void onMetadataChanged();
    void onAddStepClicked();
    void onRemoveStepClicked();
    void onMoveStepUpClicked();
    void onMoveStepDownClicked();
    void onDuplicateStepClicked();
    void onStepSelectionChanged();
    void onStepTableCellChanged(int row, int column);
    void onSaveClicked();
    void onRevertClicked();

private:
    void setupUi();
    void setupConnections();
    void populateStepsTable();
    void updateStepEditor(int row);
    void syncStepFromEditor(int row);
    void setDirty(bool dirty);

    // === Tab Widget ===
    QTabWidget* m_tabWidget = nullptr;

    // === Metadata Widgets (Tab 1) ===
    QLineEdit* m_nameEdit = nullptr;
    QLineEdit* m_descriptionEdit = nullptr;
    QLineEdit* m_requirementIdEdit = nullptr;
    QLineEdit* m_requirementLinkEdit = nullptr;
    QLineEdit* m_jiraTicketEdit = nullptr;
    QLineEdit* m_jiraLinkEdit = nullptr;
    QLineEdit* m_componentEdit = nullptr;
    QLineEdit* m_featureEdit = nullptr;
    QLineEdit* m_tagsEdit = nullptr;
    QSpinBox* m_prioritySpin = nullptr;
    QSpinBox* m_timeoutSpin = nullptr;
    QCheckBox* m_enabledCheck = nullptr;

    // === Steps Widgets (Tab 2) ===
    QTableWidget* m_stepsTable = nullptr;
    QPushButton* m_btnAddStep = nullptr;
    QPushButton* m_btnRemoveStep = nullptr;
    QPushButton* m_btnMoveUp = nullptr;
    QPushButton* m_btnMoveDown = nullptr;
    QPushButton* m_btnDuplicate = nullptr;

    // === Step Editor (Tab 2) ===
    StepEditorWidget* m_stepEditor = nullptr;

    // === Actions ===
    QPushButton* m_btnSave = nullptr;
    QPushButton* m_btnRevert = nullptr;
    QStatusBar* m_statusBar = nullptr;

    // === State ===
    QString m_currentTestCaseId;
    TestCase m_editingTestCase;
    bool m_dirty = false;
    int m_selectedStepRow = -1;
};

} // namespace TestExecutor
