//
// Created by darren on 28/06/2020.
//

#include <cstring>
#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <archive.h>
#include <archive_entry.h>
#include "isodiskimageinfo.h"
#include "diskimageinfo.h"

using namespace ReScribe;

QString IsoDiskImageInfo::description()
{
    if (cacheStale()) {
        updateCache();
    }

    return m_description;
}

QString IsoDiskImageInfo::type()
{
    return QObject::tr("ISO 9660 CD Image File");
}

bool IsoDiskImageInfo::infoIsAvailable()
{
    if (cacheStale()) {
        updateCache();
    }

    return !m_description.isEmpty();
}

bool IsoDiskImageInfo::cacheStale() const
{
    return m_description.isNull();
}

int64_t IsoDiskImageInfo::size()
{
    return QFileInfo(diskImageFile()).size();
}

void IsoDiskImageInfo::setDiskImageFile(const QString & fileName)
{
    DiskImageInfo::setDiskImageFile(fileName);
    m_description = QString();
}

void IsoDiskImageInfo::updateCache()
{
    static const char infoFile[11] = ".disk/info";

    auto * archive = archive_read_new();
    archive_read_support_filter_all(archive);
    archive_read_support_format_iso9660(archive);
    m_description = QStringLiteral("");

    if (ARCHIVE_OK != archive_read_open_filename(archive, diskImageFile().toUtf8().constData(), 10240)) {
        qDebug() << "failed to open" << diskImageFile();
        archive_read_free(archive);
        return;
    }

    struct archive_entry * archiveEntry = nullptr;

    while (ARCHIVE_OK == archive_read_next_header(archive, &archiveEntry)) {
        if (0 == std::strcmp(archive_entry_pathname(archiveEntry), infoFile)) {
            // NOTE read at most 1kb - if the file is bigger than that, it's not the file we're looking for
            char buffer[1025];

            // TODO confirm UTF8 is the correct encoding to use with this file
            auto bytes = archive_read_data(archive, buffer, 1024);

            if (0 < bytes) {
                buffer[bytes] = 0;
                qDebug() << "read .disk/info" << buffer;
                m_description = QString::fromUtf8(buffer, bytes);
                qDebug() << "QString description is now" << m_description;
            }
            break;
        }
    }

    if (ARCHIVE_OK != archive_read_free(archive)) {
        qDebug() << "failed to close" << diskImageFile() << "cleanly";
    }
}
