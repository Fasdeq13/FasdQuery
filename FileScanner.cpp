#include "FileScanner.h"
#include "CategoryFilter.h"

#include <QDirIterator>
#include <QFileInfo>

FileScanner::FileScanner(QObject *parent) : QObject(parent) {
}

void FileScanner::startScan(const QString &rootPath) {
    m_stopRequested.storeRelaxed(false);

    QVector<FileEntry> batch;
    batch.reserve(BatchSize);
    qint64 totalCount = 0;

    scanRecursive(rootPath, batch, totalCount);

    if (!batch.isEmpty() && !m_stopRequested.loadRelaxed()) {
        emit batchReady(batch);
    }

    emit scanFinished(totalCount);
}

void FileScanner::stopScan() {
    m_stopRequested.storeRelaxed(true);
}

void FileScanner::scanRecursive(const QString &path, QVector<FileEntry> &batch, qint64 &totalCount) {
    if (m_stopRequested.loadRelaxed()) {
        return;
    }

    QDirIterator it(path, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
                     QDirIterator::NoIteratorFlags);

    while (it.hasNext() && !m_stopRequested.loadRelaxed()) {
        it.next();
        const QFileInfo info = it.fileInfo();

        FileEntry entry;
        entry.name = info.fileName();
        entry.absolutePath = info.absoluteFilePath();
        entry.isDirectory = info.isDir();
        entry.size = info.isDir() ? 0 : info.size();
        entry.modified = info.lastModified();
        entry.extension = info.suffix();
        entry.category = entry.isDirectory ? CategoryNone
                                            : CategoryFilter::categoryForExtension(entry.extension);

        batch.append(entry);
        totalCount++;

        if (batch.size() >= BatchSize) {
            emit batchReady(batch);
            emit scanProgress(entry.absolutePath);
            batch.clear();
            batch.reserve(BatchSize);
        }

        if (info.isDir() && !info.isSymLink()) {
            scanRecursive(info.absoluteFilePath(), batch, totalCount);
        }
    }
}
