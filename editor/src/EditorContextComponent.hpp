#pragma once

#include "ecs/core/EntityHandle.hpp"


namespace siren::editor
{

struct EditorContextComponent final : core::Component
{
    core::EntityHandle selectedEntity = core::EntityHandle::invalid();
    EditorContextComponent() = default;
};

} // siren::editor
