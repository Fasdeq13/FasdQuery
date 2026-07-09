#include "FileTableModel.h"
#include "CategoryFilter.h"

#include <QLocale>
#include <QFileIconProvider>

FileTableModel::FileTableModel(QObject *parent) : QAbstractTableModel(parent) {
}

int FileTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_filteredIndices.size();
}

int FileTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant FileTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_filteredIndices.size()) {
        return QVariant();
    }

    const FileEntry &entry = m_allEntries.at(m_filteredIndices.at(index.row()));

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case ColumnName: return entry.name;
            case ColumnPath: return entry.absolutePath;
            case ColumnSize: return entry.isDirectory ? QString("--")
                                                        : QLocale::system().formattedDataSize(entry.size);
            case ColumnModified: return entry.modified.toString("yyyy-MM-dd HH:mm");
            default: return QVariant();
        }
    }

    if (role == Qt::DecorationRole && index.column() == ColumnIcon) {
        static QFileIconProvider provider;
        return provider.icon(entry.isDirectory ? QFileIconProvider::Folder : QFileIconProvider::File);
    }

    if (role == Qt::ToolTipRole) {
        return entry.absolutePath;
    }

    return QVariant();
}

QVariant FileTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
        case ColumnIcon: return "";
        case ColumnName: return "Name";
        case ColumnPath: return "Path";
        case ColumnSize: return "Size";
        case ColumnModified: return "Modified";
        default: return QVariant();
    }
}

void FileTableModel::appendEntries(const QVector<FileEntry> &entries) {
    if (entries.isEmpty()) return;

    m_allEntries.append(entries);
    rebuildFilteredIndices();
}

void FileTableModel::clear() {
    beginResetModel();
    m_allEntries.clear();
    m_filteredIndices.clear();
    endResetModel();
}

void FileTableModel::setSearchText(const QString &text) {
    m_searchText = text;
    rebuildFilteredIndices();
}

void FileTableModel::setCategoryMask(uint16_t mask) {
    m_categoryMask = mask;
    rebuildFilteredIndices();
}

void FileTableModel::registerOpen(const QModelIndex &index) {
    if (!index.isValid() || index.row() >= m_filteredIndices.size()) return;
    m_allEntries[m_filteredIndices.at(index.row())].openCount++;
}

const FileEntry &FileTableModel::entryAt(int row) const {
    return m_allEntries.at(m_filteredIndices.at(row));
}

int FileTableModel::totalIndexedCount() const {
    return m_allEntries.size();
}

void FileTableModel::rebuildFilteredIndices() {
    beginResetModel();

    m_filteredIndices.clear();
    m_filteredIndices.reserve(m_allEntries.size());

    for (int i = 0; i < m_allEntries.size(); ++i) {
        const FileEntry &entry = m_allEntries.at(i);

        if (!m_searchText.isEmpty() &&
            !entry.name.contains(m_searchText, Qt::CaseInsensitive)) {
            continue;
        }

        if (!entry.isDirectory && (entry.category & m_categoryMask) == 0) {
            continue;
        }

        m_filteredIndices.append(i);
    }

    std::sort(m_filteredIndices.begin(), m_filteredIndices.end(), [this](int a, int b) {
        return m_allEntries.at(a).openCount > m_allEntries.at(b).openCount;
    });

    endResetModel();
}
