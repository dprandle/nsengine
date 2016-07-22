#ifndef NSENUM_HASH
#define NSENUM_HASH

#include "nsstring.h"

struct EnumHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

#endif
