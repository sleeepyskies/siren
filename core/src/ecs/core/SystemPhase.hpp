#pragma once

namespace siren::core
{
/**
 * @brief Enum defining buckets systems can be assigned to. System buckets will be updated in the
 * order defined here.
 *
 * @note This does mean that the caller may be able to assign systems to incorrect phases, for
 * example placing the RenderSystem inside SystemPhase::LOGIC. Currently, siren just assumes the
 * caller knows well enough to not do this. In the future, it may be nice to only expose some system
 * phases, and have core systems be fixed to their phases.
 *
 * @note Systems within a bucket are assumed to be execution order independent.
 */
enum SystemPhase
{
    LogicPhase,
    RenderPhase,

    SystemPhaseMax, // do not use, just indicates amount of phases
};
} // namespace siren::ecs
