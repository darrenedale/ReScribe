//
// Created by darren on 27/06/2020.
//

#include <string>
#include <sstream>
#include <QString>
#include "sizetextgenerator.h"

using namespace ReScribe;

SizeTextGenerator::SizeTextGenerator(int64_t size)
: m_size(size),
  m_flags(0)
{
}

template<>
std::string SizeTextGenerator::text<std::string>(int flags) const {
    auto size = this->size();
    int64_t threshold = 1;
    std::ostringstream out;

    if (0 > size) {
        out << "-";
        size = -size;
    }

    if (threshold * 1024 > size) {
        out << size << " bytes";
        return out.str();
    }

    threshold *= 1024;

    if (threshold * 1024 > size) {
        out << (size / threshold) << " KiB";
        return out.str();
    }

    threshold *= 1024;

    if (threshold * 1024 > size) {
        out << (static_cast<double>(size) / threshold) << " MiB";
        return out.str();
    }

    threshold *= 1024;

    if (threshold * 1024 > size) {
        out << (static_cast<double>(size) / threshold) << " GiB";
        return out.str();
    }

    threshold *= 1024;
    out << (static_cast<double>(size) / threshold) << " TiB";
    return out.str();
}

template<>
QString SizeTextGenerator::text<QString>(int flags) const {
    auto size = this->size();
    int64_t threshold = 1;
    auto negate = QStringLiteral("");

    if (0 > size) {
        negate = QStringLiteral("-");
        size = -size;
    }
    
    if (threshold * 1024 > size) {
        return QStringLiteral("%1%2 bytes").arg(negate).arg(size);
    }

    threshold *= 1024;

    if (threshold * 1024 > size) {
        return QStringLiteral("%1%2 KiB").arg(negate).arg(size / threshold);
    }

    threshold *= 1024;

    if (threshold * 1024 > size) {
        return QStringLiteral("%1%2 MiB").arg(negate).arg(static_cast<double>(size) / threshold, 0, 'f', 1, '0');
    }

    threshold *= 1024;

    if (threshold * 1024 > size) {
        return QStringLiteral("%1%2 GiB").arg(negate).arg(static_cast<double>(size) / threshold, 0, 'f', 1, '0');
    }

    threshold *= 1024;
    return QStringLiteral("%1%2 TiB").arg(negate).arg(static_cast<double>(size) / threshold, 0, 'f', 1, '0');
}
