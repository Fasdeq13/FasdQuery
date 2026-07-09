#pragma once

#include <QMainWindow>
#include <QThread>
#include "FileScanner.h"
#include "FileTableModel.h"
#include "CategoryFilter.h"

class QLineEdit;
class QTableView;
class QPushButton;
class QLabel;
class QComboBox;
class QVBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSearchTextChanged(const QString &text);
    void onCategoryButtonToggled(uint16_t category, bool checked);
    void onBatchReady(const QVector<FileEntry> &entries);
    void onScanFinished(qint64 totalCount);
    void onScanProgress(const QString &path);
    void onOpenFile(const QModelIndex &index);
    void onContextMenuRequested(const QPoint &point);
    void onThemeChanged(int index);
    void startNewScan();

private:
    void buildUi();
    void buildCategoryBar(QVBoxLayout *parentLayout);
    void applyTheme(const QString &themeName);
    void openFile(const QString &path);
    void showInFileManager(const QString &path);
    void copyPathToClipboard(const QString &path);

    QLineEdit *m_searchBox = nullptr;
    QTableView *m_tableView = nullptr;
    QLabel *m_statusLabel = nullptr;
    QComboBox *m_themeCombo = nullptr;
    QPushButton *m_rescanButton = nullptr;

    FileTableModel *m_model = nullptr;
    FileScanner *m_scanner = nullptr;
    QThread m_scannerThread;

    uint16_t m_activeCategoryMask = CategoryAll;
    QString m_rootPath;
};
