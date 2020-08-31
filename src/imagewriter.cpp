//
// Created by darren on 12/07/2020.
//

#include <QDebug>
#include <QtGlobal>
#include <QTemporaryFile>
#include <KAuthAction>
#include <KAuthExecuteJob>
#include "imagewriter.h"

namespace
{
    // can't find a "never timeout" setting, so give the helper 1 hour to complete
    constexpr const int Timeout = 60 * 60 * 1000;
}

using namespace ReScribe;

ImageWriter::ImageWriter(QUrl url, QString device, bool decompress, bool verifyChecksum)
: m_verifyChecksum(verifyChecksum),
  m_decompress(decompress),
  m_image(std::move(url)),
  m_device(device),
  m_state(State::Ready)
{
}

void ImageWriter::connectDownloadToWrite()
{
    connect(m_downloadJob.get(), &KIO::FileCopyJob::finished, [this] () {
        m_imageFile->close();

        if (0 == m_downloadJob->error()) {
            Q_EMIT downloadComplete();
            m_state = State::Ready;
            writeImage(m_imageFile->fileName());
        } else {
            Q_EMIT downloadFailed();
            qDebug() << m_imageFile->error() << m_imageFile->errorString();
            m_state = State::Ready;
        }
    });
}

void ImageWriter::connectChecksumDownloadToImageDownload()
{
    connect(m_downloadJob.get(), &KIO::FileCopyJob::finished, [this] () {
        m_checksumFile->close();

        if (0 == m_downloadJob->error()) {
            Q_EMIT checksumDonwloaded();
            // TODO verify checksum
            m_state = State::Ready;
            downloadImage();
            connectDownloadToWrite();
        } else {
            Q_EMIT checksumVerificationUnavailable();
            qDebug() << m_imageFile->error() << m_imageFile->errorString();
            m_state = State::Ready;
        }
    });
}

void ImageWriter::write()
{
    if (imageUrl().isLocalFile()) {
        writeImage();
    } else if (verifyChecksum()) {
        downloadChecksum();
        connectChecksumDownloadToImageDownload();
    } else {
        downloadImage();
        connectDownloadToWrite();
    }
}

void ImageWriter::cancel()
{
    switch (state()) {
        case State::Ready:
            qDebug() << "No action in progress - nothing to cancel";
            break;

        case State::DownloadingChecksum:
        case State::DownloadingImage:
            Q_ASSERT(m_downloadJob.get());
            m_downloadJob->kill(KJob::KillVerbosity::EmitResult);
            break;

        case State::Writing:
            Q_ASSERT(m_writeJob.get());
            m_writeJob->kill(KJob::KillVerbosity::EmitResult);
            break;
    }
}

void ImageWriter::downloadChecksum()
{
    m_state = State::DownloadingChecksum;
}

void ImageWriter::downloadImage()
{
    Q_ASSERT(State::Ready == state());
    m_state = State::DownloadingImage;
    m_imageFile.reset(new QTemporaryFile(QStringLiteral("rescribe-remote-image-XXXXXX")));
    m_imageFile->open();
    m_downloadJob.reset(KIO::file_copy(imageUrl(), QUrl::fromLocalFile(m_imageFile->fileName()), -1, KIO::JobFlag::Overwrite | KIO::JobFlag::HideProgressInfo));

    connect(m_downloadJob.get(), qOverload<KJob *, unsigned long>(&KIO::FileCopyJob::percent), [this] (KJob *, unsigned long pc) {
        Q_EMIT downloadProgress(pc);
    });

    connect(m_downloadJob.get(), &KIO::FileCopyJob::processedSize, [this] (KJob *, qulonglong bytes) {
        Q_EMIT downloadProgress(bytes, m_downloadJob->totalAmount(KJob::Unit::Bytes));
    });
}

void ImageWriter::writeImage(QString fileName)
{
    Q_ASSERT(State::Ready == state());

    if (fileName.isEmpty()) {
        fileName = imageUrl().toLocalFile();
    }

    KAuth::Action writeAction(QStringLiteral("net.equituk.rescribe.imagewriter.write"));
    writeAction.setHelperId(QStringLiteral("net.equituk.rescribe.imagewriter"));
    writeAction.setTimeout(Timeout);

    writeAction.setArguments({
                                     {QStringLiteral("image"),  fileName},
                                     {QStringLiteral("device"), devicePath()},
                                     {QStringLiteral("decompress"), decompress()},
                             });

    m_writeJob.reset(writeAction.execute());
    connect(m_writeJob.get(), qOverload<KJob *, unsigned long>(&KAuth::ExecuteJob::percent), this, [this](KJob *, unsigned long progress) {
        writeProgress(static_cast<int>(progress));
    });

    connect(m_writeJob.get(), &KAuth::ExecuteJob::newData, [this] (const QVariantMap & data) {
        if (!data.contains(QStringLiteral("bytesWritten")) || !data.contains(QStringLiteral("totalBytes"))) {
            qDebug() << "Received unrecognised data from KAuth::ExecuteJob:" << data;
            return;
        }

        // NOTE totalBytes will be -1 if the total size is not known (i.e. we're decompressing on-the-fly)
        auto bytesWritten = data["bytesWritten"].value<qint64>();
        auto totalBytes = data["totalBytes"].value<qint64>();
        Q_EMIT writeProgress(bytesWritten, totalBytes);
    });

    connect(m_writeJob.get(), &KAuth::ExecuteJob::finished, [this] (KJob * job) {
        auto * writeJob = qobject_cast<KAuth::ExecuteJob *>(job);

        if (writeJob->error()) {
            qDebug() << "Error writing image: [" << writeJob->error() << "]" << writeJob->errorString();
            Q_EMIT writeFailed();
        } else {
            Q_EMIT writeComplete();
        }

        m_state = State::Ready;
    });

    m_state = State::Writing;
    Q_EMIT writeStarted();
    m_writeJob->start();
}

ImageWriter::~ImageWriter() = default;
