//
// Created by darren on 28/06/2020.
//

#include <QFile>
#include <QDebug>
#include "diskimageinfofactory.h"
#include "diskimageinfo.h"
#include "isodiskimageinfo.h"

using namespace ReScribe;

std::unique_ptr<DiskImageInfo> DiskImageInfoFactory::create(const QString & fileName)
{
    auto image = QFile(fileName);

    if (!image.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open" << fileName << "for reading.";
        return {};
    }

    if (isIsoDiskImage(image)) {
        auto info = std::make_unique<IsoDiskImageInfo>();
        info->setDiskImageFile(fileName);
        return info;
    }

    return {};
}

bool DiskImageInfoFactory::isIsoDiskImage(QIODevice & image)
{
    static const auto magic = QByteArray("CD001");

    if (image.isSequential()) {
        qDebug() << "IODevice is not seekable, can't determine whether it's an ISO.";
        return false;
    }

    // NOTE offsets must remain in ascending order
    for (const auto offset : {0x8001, 0x8801, 0x9001}) {
        if (!image.seek(offset)) {
            qDebug() << "Failed to seek to offset" << Qt::hex << offset << ", can't determine whether it's an ISO.";
            return false;
        }

        if (image.read(magic.size()) == magic) {
            return true;
        }
    }

    return false;
}
