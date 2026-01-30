#pragma once
#include "ecs/core/World.hpp"


namespace siren::editor
{

/**
 * @brief Contains any global Editor related state. Panels may have handles to this.
 */
struct EditorState
{
    /// @brief The currently active scene.
    /// @todo Load from file.
    core::World scene{ };
    /// @brief The currently selected entity. The handle is EntityHandle::invalid() if no entity is selected.
    core::EntityHandle selectedEntity = core::EntityHandle::invalid();
};

} // namespace siren::editor
