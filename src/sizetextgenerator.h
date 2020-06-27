//
// Created by darren on 27/06/2020.
//

#ifndef RESCRIBE_SIZETEXTGENERATOR_H
#define RESCRIBE_SIZETEXTGENERATOR_H

#include <cstdint>
#include <string>

namespace ReScribe
{
    class SizeTextGenerator
    {
    public:
        explicit SizeTextGenerator(int64_t size = 0);

        SizeTextGenerator & setFlags(int flags)
        {
            m_flags = flags;
            return *this;
        }

        int flags() const
        {
            return m_flags;
        }

        int64_t size() const
        {
            return m_size;
        }

        SizeTextGenerator & setSize(int64_t size)
        {
            m_size = size;
            return *this;
        }

        template<typename StringType = std::string>
        StringType text(int flags) const;

        template<typename StringType = std::string>
        StringType text() const {
            return text<StringType>(flags());
        }

    private:
        int64_t m_size;
        int m_flags;
    };
}

#endif //RESCRIBE_SIZETEXTGENERATOR_H
