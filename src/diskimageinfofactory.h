//
// Created by darren on 28/06/2020.
//

#ifndef RESCRIBE_DISKIMAGEINFOFACTORY_H
#define RESCRIBE_DISKIMAGEINFOFACTORY_H

#include <memory>
#include <QString>

class QIODevice;

namespace ReScribe
{
    class DiskImageInfo;

    class DiskImageInfoFactory
    {
        public:
            static std::unique_ptr<DiskImageInfo> create(const QString & fileName);

        protected:
            static bool isIsoDiskImage(QIODevice & image);
    };
}

#endif //RESCRIBE_DISKIMAGEINFOFACTORY_H
