#include "CategoryFilter.h"

#include <QSet>
#include <QHash>

uint16_t CategoryFilter::categoryForExtension(const QString &extension) {
    static const QSet<QString> audioExt = {
        "mp3", "wav", "flac", "ogg", "aac", "wma", "m4a", "opus", "aiff", "alac"
    };
    static const QSet<QString> videoExt = {
        "mp4", "mkv", "avi", "mov", "wmv", "flv", "webm", "mpeg", "mpg", "m4v", "ts"
    };
    static const QSet<QString> imageExt = {
        "png", "jpg", "jpeg", "gif", "bmp", "svg", "webp", "tiff", "ico", "heic", "raw"
    };
    static const QSet<QString> documentExt = {
        "pdf", "doc", "docx", "odt", "txt", "md", "rtf", "xls", "xlsx", "ppt", "pptx", "csv"
    };
    static const QSet<QString> archiveExt = {
        "zip", "rar", "7z", "tar", "gz", "bz2", "xz", "iso", "deb", "rpm"
    };
    static const QSet<QString> codeExt = {
        "cpp", "h", "hpp", "c", "cc", "py", "js", "ts", "java", "rs", "go", "sh",
        "json", "xml", "html", "css", "cmake", "qml", "sql", "yaml", "yml"
    };

    const QString ext = extension.toLower();

    if (audioExt.contains(ext)) return CategoryAudio;
    if (videoExt.contains(ext)) return CategoryVideo;
    if (imageExt.contains(ext)) return CategoryImage;
    if (documentExt.contains(ext)) return CategoryDocument;
    if (archiveExt.contains(ext)) return CategoryArchive;
    if (codeExt.contains(ext)) return CategoryCode;

    return CategoryOther;
}

QString CategoryFilter::iconNameForCategory(uint16_t category) {
    switch (category) {
        case CategoryAudio: return "cat-audio";
        case CategoryVideo: return "cat-video";
        case CategoryImage: return "cat-image";
        case CategoryDocument: return "cat-document";
        case CategoryArchive: return "cat-archive";
        case CategoryCode: return "cat-code";
        default: return "cat-other";
    }
}

QString CategoryFilter::displayNameForCategory(uint16_t category) {
    switch (category) {
        case CategoryAudio: return "Audio";
        case CategoryVideo: return "Video";
        case CategoryImage: return "Images";
        case CategoryDocument: return "Documents";
        case CategoryArchive: return "Archives";
        case CategoryCode: return "Code";
        default: return "Other";
    }
}
