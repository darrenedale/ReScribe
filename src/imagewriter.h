//
// Created by darren on 12/07/2020.
//

#ifndef RESCRIBE_IMAGEWRITER_H
#define RESCRIBE_IMAGEWRITER_H

#include <memory>
#include <QObject>
#include <QUrl>
#include <KIO/FileCopyJob>
#include <KAuthExecuteJob>

class QTemporaryFile;

namespace ReScribe {
    class ImageWriter : public QObject
    {
        Q_OBJECT

        public:
            enum class State
            {
                Ready = 0,
                DownloadingChecksum = 1,
                DownloadingImage = 2,
                Writing = 3,
            };

            ImageWriter(QUrl url, QString devicePath, bool decompress = true, bool verifyChecksum = true);
            virtual ~ImageWriter();

            void write();

            State state() const
            {
                return m_state;
            }

            const QUrl & imageUrl() const
            {
                return m_image;
            }

            const QString & devicePath() const
            {
                return m_device;
            }

            bool decompress() const
            {
                return m_decompress;
            }

            bool verifyChecksum() const
            {
                return m_verifyChecksum;
            }

        public Q_SLOTS:
            void cancel();

        Q_SIGNALS:
            void checksumDonwloaded();
            void checksumVerificationUnavailable();
            void checksumVerificationFailed();
            void checksumVerified();
            void downloadStarted();
            void downloadProgress(int);
            void downloadProgress(int, int);
            void downloadFailed();
            void downloadComplete();
            void writeStarted();
            void writeProgress(int);
            void writeProgress(int, int);
            void writeFailed();
            void writeComplete();

        private:
            void downloadChecksum();
            void downloadImage();
            void writeImage(QString fileName = {});

            void connectChecksumDownloadToImageDownload();
            void connectDownloadToWrite();

            bool m_verifyChecksum;
            bool m_decompress;
            QUrl m_image;
            QString m_device;
            std::unique_ptr<QTemporaryFile> m_imageFile;
            std::unique_ptr<QTemporaryFile> m_checksumFile;
            std::unique_ptr<KIO::FileCopyJob> m_downloadJob;
            std::unique_ptr<KAuth::ExecuteJob> m_writeJob;
            State m_state;
    };
}
#endif //RESCRIBE_IMAGEWRITER_H
