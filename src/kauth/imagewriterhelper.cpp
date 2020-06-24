//
// Created by darren on 23/06/2020.
//

#include "imagewriterhelper.h"
#include <QUrl>
#include <QFile>

using namespace ReScribe;

namespace
{
    constexpr const int IOBufferSize = 4096 * 1024;
}

ActionReply ImageWriterHelper::write(QVariantMap args) {
    ActionReply reply;

    auto image = args["image"].toString();
    auto device = args["device"].toString();
    auto ret = write(image, device);

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

ImageWriterHelper::ExitCode ImageWriterHelper::write(const QString &image, const QString &device)
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
        // TODO optimise for native device block size
        readBytes = imageFile.read(buffer, IOBufferSize);
        writeBytes = deviceFile.write(buffer, readBytes);

        if (writeBytes != readBytes) {
            return ExitCode::WriteError;
        }

        totalBytes += writeBytes;

        HelperSupport::progressStep(static_cast<double>(writeBytes) / static_cast<double>(totalBytes) * 100.0l);
        HelperSupport::progressStep({
            {QStringLiteral("bytesWritten"), writeBytes},
            {QStringLiteral("totalBytes"), totalBytes}
        });
    }

    return ExitCode::Ok;
}

KAUTH_HELPER_MAIN("net.equituk.rescribe.imagewriter", ReScribe::ImageWriterHelper)
