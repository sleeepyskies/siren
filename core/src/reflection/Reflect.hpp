#pragma once

#include "utilities/spch.hpp"


namespace siren::reflect
{

struct Type;
struct Field;
struct Method;

struct Field
{
    const char* name;
    size_t offset;
    const Type* type;
};

struct Method { };

template <typename T, typename... Args>
using ctorFunction = std::function<T(Args...)>;

struct Constructor { };

struct Type
{
    const char* name;
    Constructor ctor;
    Vector<Field> fields;
    Vector<Method> functions;
};

struct TypeRegistry { };

// desired api:
inline auto test() -> void
{
    struct Component;

    reflect::registry
    for (const auto& componentType : ComponentRegistry::get()) {
        if (entity.has(componentType)) {
            component = scene.get<componentType>(entity);
            drawComponentEditor(component);
        }
    }
}

} // siren::reflect

