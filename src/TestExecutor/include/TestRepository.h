#pragma once
/**
 * @file TestRepository.h
 * @brief Repository for managing test cases with JSON persistence.
 * 
 * The TestRepository is the central storage for all test cases.
 * It provides:
 * - CRUD operations for test cases and suites
 * - JSON import/export
 * - Filtering and search
 * - Tree structure for UI
 */

#include "TestDataModels.h"
#include <QObject>
#include <QAbstractItemModel>
#include <QMap>

namespace TestExecutor {

/**
 * @brief Item type for tree model
 */
enum class TreeItemType {
    Root,
    Suite,
    TestCase,
    Step
};

/**
 * @brief Tree model for displaying test cases in a hierarchical view.
 * 
 * Structure:
 * - Root
 *   - Suite 1
 *     - TestCase 1.1
 *     - TestCase 1.2
 *   - Suite 2
 *     - TestCase 2.1
 *   - Unsorted (tests not in a suite)
 *     - TestCase X
 */
class TestTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        StatusRole,
        DescriptionRole,
        RequirementRole,
        JiraRole,
        TagsRole
    };

    explicit TestTreeModel(QObject* parent = nullptr);
    ~TestTreeModel() override;

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    
    // Drag and drop support
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    // Data access
    QString itemId(const QModelIndex& index) const;
    TreeItemType itemType(const QModelIndex& index) const;
    TestCase* testCaseAt(const QModelIndex& index);
    TestSuite* testSuiteAt(const QModelIndex& index);
    
    // Rebuild model from repository
    void refresh();

private:
    struct TreeItem;
    QScopedPointer<TreeItem> m_rootItem;
    void buildTree();
};

//=============================================================================
// TestRepository - Central test storage
//=============================================================================

/**
 * @brief Central repository for all test cases and suites.
 * 
 * Provides:
 * - In-memory storage of test cases
 * - JSON file load/save
 * - CRUD operations
 * - Signals for UI updates
 */
class TestRepository : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     */
    static TestRepository& instance();

    // === File Operations ===
    
    /**
     * @brief Load test cases from a JSON file
     * @param filePath Path to the JSON file
     * @return true if successful
     */
    bool loadFromFile(const QString& filePath);
    
    /**
     * @brief Save all test cases to a JSON file
     * @param filePath Path to save to
     * @return true if successful
     */
    bool saveToFile(const QString& filePath);
    
    /**
     * @brief Import test cases from JSON (merges with existing)
     * @param filePath Path to the JSON file
     * @param overwriteExisting If true, overwrite tests with same ID
     * @return Number of tests imported
     */
    int importFromFile(const QString& filePath, bool overwriteExisting = false);
    
    /**
     * @brief Export selected test cases to JSON
     * @param testIds List of test case IDs to export
     * @param filePath Path to save to
     * @return true if successful
     */
    bool exportToFile(const QStringList& testIds, const QString& filePath) const;
    
    /**
     * @brief Get the current file path
     */
    QString currentFilePath() const { return m_currentFilePath; }
    
    /**
     * @brief Check if there are unsaved changes
     */
    bool hasUnsavedChanges() const { return m_dirty; }

    // === Test Case Operations ===
    
    /**
     * @brief Get all test cases
     */
    QList<TestCase> allTestCases() const { return m_testCases.values(); }
    
    /**
     * @brief Get test case by ID
     */
    TestCase* testCase(const QString& id);
    const TestCase* testCase(const QString& id) const;
    
    /**
     * @brief Add a new test case
     * @return true if added (false if ID already exists)
     */
    bool addTestCase(const TestCase& testCase);
    
    /**
     * @brief Update an existing test case
     * @return true if updated (false if not found)
     */
    bool updateTestCase(const TestCase& testCase);
    
    /**
     * @brief Remove a test case
     * @return true if removed
     */
    bool removeTestCase(const QString& id);
    
    /**
     * @brief Check if a test case exists
     */
    bool containsTestCase(const QString& id) const { return m_testCases.contains(id); }
    
    /**
     * @brief Get number of test cases
     */
    int testCaseCount() const { return m_testCases.size(); }

    // === Test Suite Operations ===
    
    QList<TestSuite> allTestSuites() const { return m_testSuites.values(); }
    TestSuite* testSuite(const QString& id);
    bool addTestSuite(const TestSuite& suite);
    bool updateTestSuite(const TestSuite& suite);
    bool removeTestSuite(const QString& id);

    // === Filtering ===
    
    /**
     * @brief Find test cases matching criteria
     */
    QList<TestCase> findTestCases(const QString& searchText,
                                   const QStringList& tags = {},
                                   const QString& component = QString()) const;
    
    /**
     * @brief Get test cases by tag
     */
    QList<TestCase> testCasesByTag(const QString& tag) const;
    
    /**
     * @brief Get test cases by component
     */
    QList<TestCase> testCasesByComponent(const QString& component) const;
    
    /**
     * @brief Get all unique tags
     */
    QStringList allTags() const;
    
    /**
     * @brief Get all unique components
     */
    QStringList allComponents() const;

    // === Tree Model ===
    
    /**
     * @brief Get the tree model for UI binding
     */
    TestTreeModel* treeModel() { return m_treeModel; }

    // === Utility ===
    
    /**
     * @brief Create a new empty test case with generated ID
     */
    static TestCase createNewTestCase();
    
    /**
     * @brief Clear all data
     */
    void clear();

signals:
    /**
     * @brief Emitted when a test case is added
     */
    void testCaseAdded(const QString& id);
    
    /**
     * @brief Emitted when a test case is updated
     */
    void testCaseUpdated(const QString& id);
    
    /**
     * @brief Emitted when a test case is removed
     */
    void testCaseRemoved(const QString& id);
    
    /**
     * @brief Emitted when the repository is loaded from file
     */
    void repositoryLoaded(const QString& filePath);
    
    /**
     * @brief Emitted when the repository is saved
     */
    void repositorySaved(const QString& filePath);
    
    /**
     * @brief Emitted when dirty state changes
     */
    void dirtyStateChanged(bool dirty);

private:
    TestRepository();
    ~TestRepository() override;
    TestRepository(const TestRepository&) = delete;
    TestRepository& operator=(const TestRepository&) = delete;

    void setDirty(bool dirty);

    QMap<QString, TestCase> m_testCases;
    QMap<QString, TestSuite> m_testSuites;
    QString m_currentFilePath;
    bool m_dirty = false;
    TestTreeModel* m_treeModel = nullptr;
};

} // namespace TestExecutor
