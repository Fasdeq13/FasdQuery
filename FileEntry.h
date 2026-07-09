#pragma once

#include <QString>
#include <QDateTime>
#include <cstdint>

struct FileEntry {
    QString name;
    QString absolutePath;
    QString extension;
    qint64 size = 0;
    QDateTime modified;
    uint16_t category = 0;
    bool isDirectory = false;
    int openCount = 0;
};
