//
// Created by darren on 28/06/2020.
//

#ifndef RESCRIBE_ISODISKIMAGEINFO_H
#define RESCRIBE_ISODISKIMAGEINFO_H

#include <cstdint>
#include "diskimageinfo.h"

namespace ReScribe
{
    class IsoDiskImageInfo : public DiskImageInfo
    {
    public:
        void setDiskImageFile(const QString & fileName) override;
        bool infoIsAvailable() override;
        QString type() override;
        QString description() override;
        int64_t size() override;

    private:
        bool cacheStale() const;
        void updateCache();

        QString m_description;
    };
}

#endif //RESCRIBE_ISODISKIMAGEINFO_H
