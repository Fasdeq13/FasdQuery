#include "CategoryFilter.h"
#pragma once
#include <QAbstractTableModel>
#include <QVector>
#include "FileEntry.h"

class FileTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        ColumnIcon = 0,
        ColumnName,
        ColumnPath,
        ColumnSize,
        ColumnModified,
        ColumnCount
    };

    explicit FileTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void appendEntries(const QVector<FileEntry> &entries);
    void clear();
    void setSearchText(const QString &text);
    void setCategoryMask(uint16_t mask);
    void registerOpen(const QModelIndex &index);
    const FileEntry &entryAt(int row) const;
    int totalIndexedCount() const;

private:
    void rebuildFilteredIndices();

    QVector<FileEntry> m_allEntries;
    QVector<int> m_filteredIndices;
    QString m_searchText;
    uint16_t m_categoryMask = CategoryAll;
};
