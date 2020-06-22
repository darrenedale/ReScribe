//
// Created by darren on 22/06/2020.
//

#ifndef RESCRIBE_IMAGEWRITER_H
#define RESCRIBE_IMAGEWRITER_H

#include <memory>
#include <solid/device.h>
#include <QUrl>

namespace ReScribe
{
    class ImageWriter
    {
    public:
        enum class State {
            Unknown = 0,
            Invalid,
            Ready,
            InProgress,
            Failed,
            Finished
        };

        ImageWriter(QUrl image, Solid::Device device);

        inline State state() const
        {
            return m_state;
        }

        bool validate() const;

        bool isValid() const {
            if (State::Unknown == state()) {
                return validate();
            }

            return State::Invalid != state();
        }

        bool isInProgress() const
        {
            return State::InProgress == state();
        }

        bool hasFinished() const
        {
            return State::Finished == state();
        }

        bool hasFailed() const
        {
            return State::Failed == state();
        }

        std::optional<qulonglong> imageSize() const;
        qulonglong bytesWritten() const;

        QUrl image() const
        {
            return m_image;
        }

        void setImage(QUrl url);

        Solid::Device device() const
        {
            return m_device;
        }

        void setDevice(Solid::Device device);

        bool write();

        Q_SIGNALS:
            void startedWriting();
            void progress(qulonglong bytes);
            void failedWriting();
            void finishedWriting();

        private:
            QUrl m_image;
            Solid::Device m_device;
            mutable State m_state;
    };
}

#endif //RESCRIBE_IMAGEWRITER_H
