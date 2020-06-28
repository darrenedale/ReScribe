//
// Created by darren on 28/06/2020.
//

#ifndef RESCRIBE_DISKIMAGEINFO_H
#define RESCRIBE_DISKIMAGEINFO_H

#include <QString>

namespace ReScribe {
    class DiskImageInfo
    {
    public:
        virtual bool infoIsAvailable() = 0;
        virtual QString type() = 0;
        virtual QString description() = 0;
        virtual int64_t size() = 0;

        virtual QString diskImageFile() const
        {
            return m_imageFile;
        }

        virtual void setDiskImageFile(const QString & fileName)
        {
            m_imageFile = fileName;
        }

    private:
        QString m_imageFile;
    };
}

#endif //RESCRIBE_DISKIMAGEINFO_H
