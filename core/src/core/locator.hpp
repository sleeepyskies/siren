#pragma once

#include "core/assert.hpp"


namespace siren::core
{
template <typename T>
class Locator {
public:
    static T& locate() {
        SIREN_ASSERT(m_item, "Cannot locate {}, it has not been provided.", TypeName<T>::value());
        return *static_cast<T*>(m_item);
    }

private:
    friend class App;

    static void provide(T* item) {
        SIREN_ASSERT(!m_item, "Attempting to provide {} more than once.", TypeName<T>::value());
        m_item = item;
    }

    static void terminate() {
        if (!m_item) { return; }
        m_item->~T();
        delete m_item;
    }

    static inline T* m_item;
};
}
