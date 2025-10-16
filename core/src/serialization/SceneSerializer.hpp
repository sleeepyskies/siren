#pragma once

#include "ecs/core/Scene.hpp"

namespace siren::serial
{

class SceneSerializer
{
public:
    explicit SceneSerializer(const Ref<ecs::Scene>& scene) : m_context(scene)
    {
    }
    virtual ~SceneSerializer() = default;

    virtual bool serialize(const Path& path) = 0;

protected:
    Ref<ecs::Scene> m_context = nullptr;
};

class SceneDeserializer
{
public:
    explicit SceneDeserializer(const Ref<ecs::Scene>& scene) : m_context(scene)
    {
    }
    virtual ~SceneDeserializer() = default;

    virtual bool deserialize(const Path& path) = 0;

protected:
    Ref<ecs::Scene> m_context = nullptr;
};

} // namespace siren::serial