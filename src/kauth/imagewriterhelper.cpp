/**
 * KAuth helper to perform privileged write of image file to device
 */

#include "imagewriterhelper.h"
#include <unistd.h>
#include <QDebug>
#include <QUrl>
#include <QFile>

namespace
{
    // decent amount of anecdotal evidence suggests 64k is a good block size
    constexpr const int IOBufferSize = 64 * 1024;
}

using namespace ReScribe;

ActionReply ImageWriterHelper::write(QVariantMap args) {
    ActionReply reply;
    auto image = args["image"].toString();
    auto device = args["device"].toString();
    auto result = writeImage(image, device);

    if (ExitCode::Ok != result) {
        reply.setType(ActionReply::Type::HelperErrorType);
        reply.setError(static_cast<int>(result));

        switch (result) {
            case ExitCode::WriteError:
                reply.setErrorDescription(tr("There was an IO error writing to the device %1.").arg(device));
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

    if (!imageFile.open(QIODevice::ReadOnly)) {
        return ExitCode::FailedToOpenImageFile;
    }

    auto deviceFile = QFile(device);

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

        // NOTE we sync otherwise job will be reported as complete way before all the writes have been committed
        fsync(deviceFile.handle());
    }

    return ExitCode::Ok;
}

KAUTH_HELPER_MAIN("net.equituk.rescribe.imagewriter", ReScribe::ImageWriterHelper)
