#pragma once

#include "ecs/core/Scene.hpp"

namespace siren::core
{

class SceneSerializer
{
public:
    explicit SceneSerializer(const Ref<Scene>& scene) : m_context(scene)
    {
    }

    virtual ~SceneSerializer() = default;

    virtual bool serialize(const Path& path) = 0;

protected:
    Ref<Scene> m_context = nullptr;
};

class SceneDeserializer
{
public:
    explicit SceneDeserializer(const Ref<Scene>& scene) : m_context(scene)
    {
    }

    virtual ~SceneDeserializer() = default;

    virtual bool deserialize(const Path& path) = 0;

protected:
    Ref<Scene> m_context = nullptr;
};

} // namespace siren::serial