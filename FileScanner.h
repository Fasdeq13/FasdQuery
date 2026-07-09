#pragma once

#include <QObject>
#include <QVector>
#include <QAtomicInteger>
#include "FileEntry.h"

class FileScanner : public QObject {
    Q_OBJECT

public:
    explicit FileScanner(QObject *parent = nullptr);

public slots:
    void startScan(const QString &rootPath);
    void stopScan();

signals:
    void batchReady(const QVector<FileEntry> &entries);
    void scanFinished(qint64 totalCount);
    void scanProgress(const QString &currentPath);

private:
    void scanRecursive(const QString &path, QVector<FileEntry> &batch, qint64 &totalCount);

    QAtomicInteger<bool> m_stopRequested{false};
    static constexpr int BatchSize = 500;
};
