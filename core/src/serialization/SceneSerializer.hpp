#pragma once

#include "ecs/core/World.hpp"

namespace siren::core
{

class SceneSerializer
{
public:
    explicit SceneSerializer(const Ref<World>& scene) : m_context(scene)
    {
    }

    virtual ~SceneSerializer() = default;

    virtual bool serialize(const Path& path) = 0;

protected:
    Ref<World> m_context = nullptr;
};

class SceneDeserializer
{
public:
    explicit SceneDeserializer(const Ref<World>& scene) : m_context(scene)
    {
    }

    virtual ~SceneDeserializer() = default;

    virtual bool deserialize(const Path& path) = 0;

protected:
    Ref<World> m_context = nullptr;
};

} // namespace siren::serial