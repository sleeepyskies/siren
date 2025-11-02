#pragma once

#include <string>
#include <type_traits>
#include <typeinfo>

#ifdef __GNUC__   // GCC or Clang
#include <cxxabi.h>
#endif

namespace siren::core
{

template <typename T>
    requires(std::is_class_v<T>)
std::string getClassName()
{
#ifdef __GNUC__
    int status;
    char* demangled  = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    std::string name = (status == 0) ? demangled : typeid(T).name();
    std::free(demangled);
    return name;
#elif defined(_MSC_VER)
    return typeid(T).name(); // usually readable on MSVC
#else
    return typeid(T).name(); // fallback
#endif
}

}
