#pragma once

#include <QString>
#include <cstdint>

enum FileCategory : uint16_t {
    CategoryNone      = 0x00,
    CategoryAudio     = 0x01,
    CategoryVideo     = 0x02,
    CategoryImage     = 0x04,
    CategoryDocument  = 0x08,
    CategoryArchive   = 0x10,
    CategoryCode      = 0x20,
    CategoryOther     = 0x40,
    CategoryAll       = 0x7F
};

class CategoryFilter {
public:
    static uint16_t categoryForExtension(const QString &extension);
    static QString iconNameForCategory(uint16_t category);
    static QString displayNameForCategory(uint16_t category);
};
