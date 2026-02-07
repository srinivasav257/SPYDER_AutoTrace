#pragma once
/**
 * @file TestExplorerPanel.h
 * @brief Test Explorer panel for browsing and managing test cases.
 * 
 * Features:
 * - Tree view of test suites and test cases
 * - Import/Export test files (JSON)
 * - Add/Remove test cases
 * - Search and filter
 * - Drag-drop reordering
 * - Context menu for actions
 */

#include <QObject>
#include <QWidget>
#include <QTreeView>
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QSet>

namespace TestExecutor {

class TestTreeModel;

/**
 * @brief Panel for exploring and managing test cases.
 */
class TestExplorerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TestExplorerPanel(QWidget* parent = nullptr);
    ~TestExplorerPanel() override;

    /**
     * @brief Get selected test case IDs
     */
    QStringList selectedTestCaseIds() const;
    
    /**
     * @brief Select a test case by ID
     */
    void selectTestCase(const QString& id);
    
    /**
     * @brief Expand all nodes
     */
    void expandAll();
    
    /**
     * @brief Collapse all nodes
     */
    void collapseAll();

signals:
    /**
     * @brief Emitted when a test case is double-clicked (open in editor)
     */
    void testCaseDoubleClicked(const QString& testCaseId);
    
    /**
     * @brief Emitted when selection changes
     */
    void selectionChanged(const QStringList& selectedIds);
    
    /**
     * @brief Emitted when user requests to run selected tests
     */
    void runSelectedRequested(const QStringList& testCaseIds);
    
    /**
     * @brief Emitted when user requests to create new test
     */
    void newTestRequested();

private slots:
    void onImportClicked();
    void onExportClicked();
    void onAddGroupClicked();
    void onAddFeatureClicked();
    void onAddTestClicked();
    void onRemoveTestClicked();
    void onSearchTextChanged(const QString& text);
    void onTreeDoubleClicked(const QModelIndex& index);
    void onTreeContextMenu(const QPoint& pos);
    void onRunSelectedClicked();

private:
    void setupUi();
    void setupConnections();
    void createContextMenu();
    void resolveSelectionContext(QString& groupName, QString& featureName) const;
    void collectTestCaseIdsFromSourceIndex(const QModelIndex& sourceIndex, QStringList& ids, QSet<QString>& seen) const;

    QTreeView* m_treeView = nullptr;
    QLineEdit* m_searchBox = nullptr;
    QPushButton* m_btnImport = nullptr;
    QPushButton* m_btnExport = nullptr;
    QPushButton* m_btnAddGroup = nullptr;
    QPushButton* m_btnAddFeature = nullptr;
    QPushButton* m_btnAddTest = nullptr;
    QPushButton* m_btnRemove = nullptr;
    QPushButton* m_btnExpand = nullptr;
    QPushButton* m_btnCollapse = nullptr;
    QPushButton* m_btnRun = nullptr;
    
    QSortFilterProxyModel* m_proxyModel = nullptr;
    QMenu* m_contextMenu = nullptr;
};

/**
 * @brief Factory function to create the panel for PanelRegistry
 */
QWidget* createTestExplorerPanel(QWidget* parent);

} // namespace TestExecutor
