//
// Created by darren on 23/06/2020.
//

#include "imagewriterhelper.h"
#include <unistd.h>
#include <QDebug>
#include <QUrl>
#include <QFile>

namespace
{
    // decent amount of anecdotal evidence suggests 64k is a decent block size
    constexpr const int IOBufferSize = 64 * 1024;
}

ActionReply ImageWriterHelper::write(QVariantMap args) {
    ActionReply reply;

    auto image = args["image"].toString();
    auto device = args["device"].toString();

    qDebug() << "Image:" << image;
    qDebug() << "Device:" << device;

    auto ret = writeImage(image, device);

    qDebug() << "writeImage() returned" << static_cast<int>(ret);
    if (ExitCode::Ok != ret) {
        reply.setType(ActionReply::Type::HelperErrorType);
        reply.setError(static_cast<int>(ret));

        switch (ret) {
            case ExitCode::NotSupported:
                reply.setErrorDescription(tr("Writing remote images is not yet supported."));
                break;

            case ExitCode::FailedToOpenImageFile:
                reply.setErrorDescription(tr("Image %1 could not be opened for reading.").arg(image));
                break;

            case ExitCode::FailedToOpenDevice:
                reply.setErrorDescription(tr("Device %1 could not be opened for writing.").arg(device));
                break;
        }
    }

    return reply;
}

ImageWriterHelper::ExitCode ImageWriterHelper::writeImage(const QString &image, const QString &device)
{
    auto imageFile = QFile(image);

    qDebug() << "opening image file";

    if (!imageFile.open(QIODevice::ReadOnly)) {
        return ExitCode::FailedToOpenImageFile;
    }

    auto deviceFile = QFile(device);

    qDebug() << "opening device file";

    if (!deviceFile.open(QIODevice::WriteOnly)) {
        return ExitCode::FailedToOpenDevice;
    }

    char buffer[IOBufferSize];
    auto totalBytes = imageFile.size();
    decltype(totalBytes) cumulativeBytes = 0;
    decltype(totalBytes) readBytes = 0;
    decltype(totalBytes) writeBytes = 0;

    HelperSupport::progressStep(0);
    HelperSupport::progressStep({
        {QStringLiteral("bytesWritten"), static_cast<qint64>(0)},
        {QStringLiteral("totalBytes"), imageFile.size()}
    });

    while (!imageFile.atEnd()) {
        readBytes = imageFile.read(buffer, IOBufferSize);
        writeBytes = deviceFile.write(buffer, readBytes);

        if (writeBytes != readBytes) {
            qDebug() << "read" << readBytes << "bytes but wrote" << writeBytes << "bytes";
            return ExitCode::WriteError;
        }

        cumulativeBytes += writeBytes;

        HelperSupport::progressStep(static_cast<double>(cumulativeBytes) / static_cast<double>(totalBytes) * 100.0l);
        HelperSupport::progressStep({
            {QStringLiteral("bytesWritten"), cumulativeBytes},
            {QStringLiteral("totalBytes"), totalBytes}
        });

        // TODO we need to sync otherwise job is reported as complete way before all the writes have actually been
        //  committed to the device, but do we need to flush on every iteration?
        fsync(deviceFile.handle());
    }

    return ExitCode::Ok;
}

KAUTH_HELPER_MAIN("net.equituk.rescribe.imagewriter", ImageWriterHelper)
