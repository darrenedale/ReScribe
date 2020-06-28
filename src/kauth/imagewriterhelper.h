/**
 * KAuth helper to perform privileged write of image file to device
 */

#ifndef RESCRIBE_IMAGEWRITERHELPER_H
#define RESCRIBE_IMAGEWRITERHELPER_H

#include <kauth.h>
#include <QObject>


namespace ReScribe
{
    using namespace KAuth;

    class ImageWriterHelper : public QObject
    {
    Q_OBJECT

    public:
        enum class ExitCode
        {
            Ok = 0,
            FailedToOpenImageFile,
            FailedToOpenDevice,
            WriteError,
        };

    public Q_SLOTS:

        ActionReply write(QVariantMap args);

    private:
        ExitCode static writeImage(const QString & image, const QString & device);
    };
}

#endif //RESCRIBE_IMAGEWRITERHELPER_H
