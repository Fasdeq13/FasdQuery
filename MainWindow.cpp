#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QProcess>
#include <QMetaObject>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_model = new FileTableModel(this);
    m_scanner = new FileScanner();
    m_scanner->moveToThread(&m_scannerThread);

    connect(&m_scannerThread, &QThread::finished, m_scanner, &QObject::deleteLater);
    connect(m_scanner, &FileScanner::batchReady, this, &MainWindow::onBatchReady);
    connect(m_scanner, &FileScanner::scanFinished, this, &MainWindow::onScanFinished);
    connect(m_scanner, &FileScanner::scanProgress, this, &MainWindow::onScanProgress);

    m_scannerThread.start();

    buildUi();
    applyTheme("Dark");

    m_rootPath = QDir::homePath();
    startNewScan();
}

MainWindow::~MainWindow() {
    m_scanner->stopScan();
    m_scannerThread.quit();
    m_scannerThread.wait();
}

void MainWindow::buildUi() {
    setWindowTitle("FasdQuery");
    resize(1100, 700);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    QHBoxLayout *topBar = new QHBoxLayout();
    m_searchBox = new QLineEdit(central);
    m_searchBox->setPlaceholderText("Search files...");
    m_searchBox->setClearButtonEnabled(true);
    connect(m_searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    m_rescanButton = new QPushButton("Rescan", central);
    connect(m_rescanButton, &QPushButton::clicked, this, &MainWindow::startNewScan);

    m_themeCombo = new QComboBox(central);
    m_themeCombo->addItems({"Dark", "Light", "Nord", "Solarized"});
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onThemeChanged);

    topBar->addWidget(m_searchBox, 1);
    topBar->addWidget(m_rescanButton);
    topBar->addWidget(m_themeCombo);
    mainLayout->addLayout(topBar);

    buildCategoryBar(mainLayout);

    m_tableView = new QTableView(central);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSortingEnabled(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColumnIcon, QHeaderView::Fixed);
    m_tableView->setColumnWidth(FileTableModel::ColumnIcon, 36);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColumnName, QHeaderView::Interactive);
    m_tableView->setColumnWidth(FileTableModel::ColumnName, 260);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColumnPath, QHeaderView::Stretch);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->verticalHeader()->setDefaultSectionSize(28);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onOpenFile);
    connect(m_tableView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onContextMenuRequested);

    mainLayout->addWidget(m_tableView, 1);

    m_statusLabel = new QLabel("Ready", central);
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(central);
}

void MainWindow::buildCategoryBar(QVBoxLayout *parentLayout) {
    QWidget *bar = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(bar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    struct CategoryDef {
        uint16_t mask;
        QString label;
    };

    const QVector<CategoryDef> categories = {
        {CategoryAudio, "Audio"},
        {CategoryVideo, "Video"},
        {CategoryImage, "Images"},
        {CategoryDocument, "Documents"},
        {CategoryArchive, "Archives"},
        {CategoryCode, "Code"},
        {CategoryOther, "Other"}
    };

    for (const CategoryDef &def : categories) {
        QPushButton *btn = new QPushButton(def.label, bar);
        btn->setCheckable(true);
        btn->setChecked(true);
        connect(btn, &QPushButton::toggled, this, [this, def](bool checked) {
            onCategoryButtonToggled(def.mask, checked);
        });
        layout->addWidget(btn);
    }

    layout->addStretch(1);
    parentLayout->addWidget(bar);
}

void MainWindow::applyTheme(const QString &themeName) {
    QString styleSheet;

    if (themeName == "Dark") {
        styleSheet = R"(
            QWidget { background-color: #1e1f26; color: #e6e6e6; font-size: 13px; }
            QLineEdit, QComboBox { background-color: #2a2b34; border: 1px solid #3a3b46; border-radius: 6px; padding: 6px; }
            QPushButton { background-color: #2a2b34; border: 1px solid #3a3b46; border-radius: 6px; padding: 6px 12px; }
            QPushButton:checked { background-color: #4F7CFF; border-color: #4F7CFF; }
            QTableView { background-color: #23242c; alternate-background-color: #262731; gridline-color: #33343e; }
            QHeaderView::section { background-color: #2a2b34; border: none; padding: 6px; }
        )";
    } else if (themeName == "Light") {
        styleSheet = R"(
            QWidget { background-color: #f5f5f7; color: #1c1c1e; font-size: 13px; }
            QLineEdit, QComboBox { background-color: #ffffff; border: 1px solid #d0d0d5; border-radius: 6px; padding: 6px; }
            QPushButton { background-color: #ffffff; border: 1px solid #d0d0d5; border-radius: 6px; padding: 6px 12px; }
            QPushButton:checked { background-color: #4F7CFF; color: white; border-color: #4F7CFF; }
            QTableView { background-color: #ffffff; alternate-background-color: #f0f0f3; gridline-color: #e2e2e6; }
            QHeaderView::section { background-color: #eeeef1; border: none; padding: 6px; }
        )";
    } else if (themeName == "Nord") {
        styleSheet = R"(
            QWidget { background-color: #2e3440; color: #eceff4; font-size: 13px; }
            QLineEdit, QComboBox { background-color: #3b4252; border: 1px solid #4c566a; border-radius: 6px; padding: 6px; }
            QPushButton { background-color: #3b4252; border: 1px solid #4c566a; border-radius: 6px; padding: 6px 12px; }
            QPushButton:checked { background-color: #88c0d0; color: #2e3440; border-color: #88c0d0; }
            QTableView { background-color: #3b4252; alternate-background-color: #434c5e; gridline-color: #4c566a; }
            QHeaderView::section { background-color: #434c5e; border: none; padding: 6px; }
        )";
    } else {
        styleSheet = R"(
            QWidget { background-color: #fdf6e3; color: #657b83; font-size: 13px; }
            QLineEdit, QComboBox { background-color: #eee8d5; border: 1px solid #93a1a1; border-radius: 6px; padding: 6px; }
            QPushButton { background-color: #eee8d5; border: 1px solid #93a1a1; border-radius: 6px; padding: 6px 12px; }
            QPushButton:checked { background-color: #268bd2; color: #fdf6e3; border-color: #268bd2; }
            QTableView { background-color: #eee8d5; alternate-background-color: #e4ddc5; gridline-color: #d3cbb0; }
            QHeaderView::section { background-color: #e4ddc5; border: none; padding: 6px; }
        )";
    }

    qApp->setStyleSheet(styleSheet);
}

void MainWindow::onSearchTextChanged(const QString &text) {
    m_model->setSearchText(text);
    m_statusLabel->setText(QString("Showing %1 of %2 items")
        .arg(m_model->rowCount())
        .arg(m_model->totalIndexedCount()));
}

void MainWindow::onCategoryButtonToggled(uint16_t category, bool checked) {
    if (checked) {
        m_activeCategoryMask |= category;
    } else {
        m_activeCategoryMask &= ~category;
    }
    m_model->setCategoryMask(m_activeCategoryMask);
}

void MainWindow::onBatchReady(const QVector<FileEntry> &entries) {
    m_model->appendEntries(entries);
}

void MainWindow::onScanFinished(qint64 totalCount) {
    m_statusLabel->setText(QString("Scan complete — %1 items indexed").arg(totalCount));
}

void MainWindow::onScanProgress(const QString &path) {
    m_statusLabel->setText(QString("Scanning: %1").arg(path));
}

void MainWindow::onOpenFile(const QModelIndex &index) {
    if (!index.isValid()) return;
    const FileEntry &entry = m_model->entryAt(index.row());
    m_model->registerOpen(index);
    openFile(entry.absolutePath);
}

void MainWindow::onContextMenuRequested(const QPoint &point) {
    QModelIndex index = m_tableView->indexAt(point);
    if (!index.isValid()) return;

    const FileEntry &entry = m_model->entryAt(index.row());

    QMenu menu(this);
    QAction *openAction = menu.addAction("Open file");
    QAction *showAction = menu.addAction("Show in file manager");
    QAction *copyAction = menu.addAction("Copy path");
    menu.addSeparator();
    QAction *deleteAction = menu.addAction("Move to trash");

    QAction *selected = menu.exec(m_tableView->viewport()->mapToGlobal(point));

    if (selected == openAction) {
        m_model->registerOpen(index);
        openFile(entry.absolutePath);
    } else if (selected == showAction) {
        showInFileManager(entry.absolutePath);
    } else if (selected == copyAction) {
        copyPathToClipboard(entry.absolutePath);
    } else if (selected == deleteAction) {
        QFile::moveToTrash(entry.absolutePath);
        startNewScan();
    }
}

void MainWindow::onThemeChanged(int index) {
    applyTheme(m_themeCombo->itemText(index));
}

void MainWindow::startNewScan() {
    m_model->clear();
    m_statusLabel->setText("Starting scan...");
    QMetaObject::invokeMethod(m_scanner, "startScan", Qt::QueuedConnection,
                               Q_ARG(QString, m_rootPath));
}

void MainWindow::openFile(const QString &path) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::showInFileManager(const QString &path) {
    QFileInfo info(path);
    const QString dirPath = info.isDir() ? info.absoluteFilePath() : info.absolutePath();

    if (QFile::exists("/usr/bin/dbus-send")) {
        QProcess process;
        process.start("dbus-send", {
            "--session", "--print-reply",
            "--dest=org.freedesktop.FileManager1",
            "/org/freedesktop/FileManager1",
            "org.freedesktop.FileManager1.ShowItems",
            QString("array:string:file://%1").arg(info.absoluteFilePath()),
            "string:"
        });
        if (process.waitForStarted(500)) {
            process.waitForFinished(2000);
            return;
        }
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}

void MainWindow::copyPathToClipboard(const QString &path) {
    QApplication::clipboard()->setText(path);
    m_statusLabel->setText("Path copied to clipboard");
}
