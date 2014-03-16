
#pragma once

#include <algorithm>

namespace stl_utils
{
    template<class T, class V>
    bool find(T& container, const V& val)
    {
        return (std::find(container.begin(), container.end(), val) != container.end());
    }

    template<class T, class V>
    void remove(T& container, const V& val)
    {
        std::remove(container.begin(), container.end(), val);
    }
}
