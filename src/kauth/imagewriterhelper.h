//
// Created by darren on 23/06/2020.
//

#ifndef RESCRIBE_IMAGEWRITERHELPER_H
#define RESCRIBE_IMAGEWRITERHELPER_H

#include <kauth.h>
#include <QObject>

    using namespace KAuth;

    class ImageWriterHelper : public QObject
    {
    Q_OBJECT

    public:
        enum class ExitCode
        {
            Ok = 0,
            NotSupported,
            FailedToOpenImageFile,
            FailedToOpenDevice,
            WriteError,
        };

    public Q_SLOTS:

        ActionReply write(QVariantMap args);

    private:
        ExitCode static writeImage(const QString & image, const QString & device);
    };

#endif //RESCRIBE_IMAGEWRITERHELPER_H